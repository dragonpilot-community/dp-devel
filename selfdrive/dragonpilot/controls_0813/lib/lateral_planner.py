import numpy as np
from common.realtime import sec_since_boot, DT_MDL
from common.numpy_fast import interp
from system.swaglog import cloudlog
from selfdrive.dragonpilot.controls_0813.lib.lateral_mpc_lib.lat_mpc import LateralMpc
from selfdrive.controls.lib.drive_helpers import CONTROL_N, MPC_COST_LAT, LAT_MPC_N, CAR_ROTATION_RADIUS, get_lane_laneless_mode
from selfdrive.controls.lib.lane_planner import LanePlanner, TRAJECTORY_SIZE
from selfdrive.controls.lib.desire_helper import DesireHelper
import cereal.messaging as messaging
from cereal import log
from selfdrive.hardware import TICI
from common.params import Params


class LateralPlanner:
  def __init__(self, CP):
    self.LP = LanePlanner(False if not TICI else Params().get_bool('WideCameraOnly'))
    self.DH = DesireHelper()

    self.last_cloudlog_t = 0
    try:
      self.steer_rate_cost = float(Params().get("dp_lateral_steer_rate_cost").decode('utf-8'))
    except:
      self.steer_rate_cost = 1.0
    self.solution_invalid_cnt = 0

    self.path_xyz = np.zeros((TRAJECTORY_SIZE, 3))
    self.path_xyz_stds = np.ones((TRAJECTORY_SIZE, 3))
    self.plan_yaw = np.zeros((TRAJECTORY_SIZE,))
    self.t_idxs = np.arange(TRAJECTORY_SIZE)
    self.y_pts = np.zeros(TRAJECTORY_SIZE)
    self.d_path_xyz = np.zeros((TRAJECTORY_SIZE, 3))

    self.lat_mpc = LateralMpc()
    self.reset_mpc(np.zeros(4))

    # dp
    self.dp_lanelines_enable = False
    self.dp_lanelines_active = False
    self.dp_camera_offset = 4 if TICI else -6
    self.dp_path_offset = 4 if TICI else 0

  def reset_mpc(self, x0=np.zeros(4)):
    self.x0 = x0
    self.lat_mpc.reset(x0=self.x0)

  def update(self, sm):
    v_ego = sm['carState'].vEgo
    measured_curvature = sm['controlsState'].curvature
    if sm.updated['dragonConf']:
      self.dp_lanelines_enable = sm['dragonConf'].dpLateralLanelines
      self.dp_camera_offset = sm['dragonConf'].dpLateralCameraOffset
      self.dp_path_offset = sm['dragonConf'].dpLateralPathOffset
      if sm['dragonConf'].dpLateralAltLanelines and sm['controlsState'].dpLateralAltActive:
        self.dp_lanelines_enable = True
        self.dp_camera_offset = sm['dragonConf'].dpLateralAltCameraOffset
        self.dp_path_offset = sm['dragonConf'].dpLateralAltPathOffset

    # Parse model predictions
    md = sm['modelV2']
    # update camera/path offset to lane planner
    self.LP.update_dp_camera_offsets(self.dp_camera_offset, self.dp_path_offset)
    # Parse model predictions
    self.LP.parse_model(md)
    if len(md.position.x) == TRAJECTORY_SIZE and len(md.orientation.x) == TRAJECTORY_SIZE:
      self.path_xyz = np.column_stack([md.position.x, md.position.y, md.position.z])
      self.t_idxs = np.array(md.position.t)
      self.plan_yaw = list(md.orientation.z)
    if len(md.position.xStd) == TRAJECTORY_SIZE:
      self.path_xyz_stds = np.column_stack([md.position.xStd, md.position.yStd, md.position.zStd])

    # Lane change logic
    lane_change_prob = self.LP.l_lane_change_prob + self.LP.r_lane_change_prob
    self.DH.update(sm['carState'], sm['carControl'].latActive, lane_change_prob, sm['dragonConf'], md)

    # Turn off lanes during lane change
    if self.DH.desire == log.LateralPlan.Desire.laneChangeRight or self.DH.desire == log.LateralPlan.Desire.laneChangeLeft:
      self.LP.lll_prob *= self.DH.lane_change_ll_prob
      self.LP.rll_prob *= self.DH.lane_change_ll_prob

    # dynamic laneline/laneless logic
    # decide if we want to use lanelines or laneless
    if self.dp_lanelines_enable:
      self.dp_lanelines_active = get_lane_laneless_mode(self.LP.lll_prob, self.LP.rll_prob, self.dp_lanelines_active)
    else:
      self.dp_lanelines_active = False

    # Calculate final driving path and set MPC costs
    if self.dp_lanelines_active:
      self.d_path_xyz = self.LP.get_d_path(v_ego, self.t_idxs, self.path_xyz)
      self.lat_mpc.set_weights(MPC_COST_LAT.PATH, MPC_COST_LAT.HEADING, self.steer_rate_cost)
    else:
      self.d_path_xyz = self.path_xyz
      path_cost = np.clip(abs(self.path_xyz[0, 1] / self.path_xyz_stds[0, 1]), 0.5, 1.5) * MPC_COST_LAT.PATH
      # Heading cost is useful at low speed, otherwise end of plan can be off-heading
      heading_cost = interp(v_ego, [5.0, 10.0], [MPC_COST_LAT.HEADING, 0.0])
      self.lat_mpc.set_weights(path_cost, heading_cost, self.steer_rate_cost)

    y_pts = np.interp(v_ego * self.t_idxs[:LAT_MPC_N + 1], np.linalg.norm(self.d_path_xyz, axis=1), self.d_path_xyz[:, 1])
    heading_pts = np.interp(v_ego * self.t_idxs[:LAT_MPC_N + 1], np.linalg.norm(self.path_xyz, axis=1), self.plan_yaw)
    self.y_pts = y_pts

    assert len(y_pts) == LAT_MPC_N + 1
    assert len(heading_pts) == LAT_MPC_N + 1
    # self.x0[4] = v_ego
    p = np.array([v_ego, CAR_ROTATION_RADIUS])
    self.lat_mpc.run(self.x0,
                     p,
                     y_pts,
                     heading_pts)
    # init state for next
    self.x0[3] = interp(DT_MDL, self.t_idxs[:LAT_MPC_N + 1], self.lat_mpc.x_sol[:, 3])

    #  Check for infeasible MPC solution
    mpc_nans = np.isnan(self.lat_mpc.x_sol[:, 3]).any()
    t = sec_since_boot()
    if mpc_nans or self.lat_mpc.solution_status != 0:
      self.reset_mpc()
      self.x0[3] = measured_curvature
      if t > self.last_cloudlog_t + 5.0:
        self.last_cloudlog_t = t
        cloudlog.warning("Lateral mpc - nan: True")

    if self.lat_mpc.cost > 20000. or mpc_nans:
      self.solution_invalid_cnt += 1
    else:
      self.solution_invalid_cnt = 0

  def publish(self, sm, pm):
    plan_solution_valid = self.solution_invalid_cnt < 2
    plan_send = messaging.new_message('lateralPlan')
    plan_send.valid = sm.all_checks(service_list=['carState', 'controlsState', 'modelV2'])

    lateralPlan = plan_send.lateralPlan
    lateralPlan.modelMonoTime = sm.logMonoTime['modelV2']
    lateralPlan.dPathPoints = self.y_pts.tolist()
    lateralPlan.psis = self.lat_mpc.x_sol[0:CONTROL_N, 2].tolist()
    lateralPlan.curvatures = self.lat_mpc.x_sol[0:CONTROL_N, 3].tolist()
    lateralPlan.curvatureRates = [float(x) for x in self.lat_mpc.u_sol[0:CONTROL_N - 1]] + [0.0]

    lateralPlan.mpcSolutionValid = bool(plan_solution_valid)
    lateralPlan.solverExecutionTime = self.lat_mpc.solve_time

    lateralPlan.desire = self.DH.desire
    lateralPlan.useLaneLines = self.dp_lanelines_active
    lateralPlan.laneChangeState = self.DH.lane_change_state
    lateralPlan.laneChangeDirection = self.DH.lane_change_direction

    plan_send.lateralPlan.dPathWLinesX = [float(x) for x in self.d_path_xyz[:, 0]]
    plan_send.lateralPlan.dPathWLinesY = [float(y) for y in self.d_path_xyz[:, 1]]

    pm.send('lateralPlan', plan_send)
