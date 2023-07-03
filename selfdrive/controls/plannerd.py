#!/usr/bin/env python3
import os
import numpy as np
from cereal import car
from common.params import Params
from common.realtime import Priority, config_realtime_process
from system.swaglog import cloudlog
from selfdrive.modeld.constants import T_IDXS
from selfdrive.controls.lib.longitudinal_planner import LongitudinalPlanner
from selfdrive.controls.lib.lateral_planner import LateralPlanner
import cereal.messaging as messaging

from selfdrive.dragonpilot.controls_0813.lib.lateral_planner import LateralPlanner as DPLateralPlanner
from selfdrive.dragonpilot.controls_0816.lib.lateral_planner import LateralPlanner as FPLateralPlanner

def cumtrapz(x, t):
  return np.concatenate([[0], np.cumsum(((x[0:-1] + x[1:])/2) * np.diff(t))])

def publish_ui_plan(sm, pm, lateral_planner, longitudinal_planner):
  plan_odo = cumtrapz(longitudinal_planner.v_desired_trajectory_full, T_IDXS)
  model_odo = cumtrapz(lateral_planner.v_plan, T_IDXS)

  ui_send = messaging.new_message('uiPlan')
  ui_send.valid = sm.all_checks(service_list=['carState', 'controlsState', 'modelV2'])
  uiPlan = ui_send.uiPlan
  uiPlan.frameId = sm['modelV2'].frameId
  uiPlan.position.x = np.interp(plan_odo, model_odo, lateral_planner.lat_mpc.x_sol[:,0]).tolist()
  uiPlan.position.y = np.interp(plan_odo, model_odo, lateral_planner.lat_mpc.x_sol[:,1]).tolist()
  uiPlan.position.z = np.interp(plan_odo, model_odo, lateral_planner.path_xyz[:,2]).tolist()
  uiPlan.accel = longitudinal_planner.a_desired_trajectory_full.tolist()
  pm.send('uiPlan', ui_send)

def plannerd_thread(sm=None, pm=None):
  config_realtime_process(5, Priority.CTRL_LOW)

  cloudlog.info("plannerd is waiting for CarParams")
  params = Params()
  CP = car.CarParams.from_bytes(params.get("CarParams", block=True))
  cloudlog.info("plannerd got CarParams: %s", CP.carName)

  debug_mode = bool(int(os.getenv("DEBUG", "0")))

  longitudinal_planner = LongitudinalPlanner(CP)
  try:
    lat_version = int(params.get('dp_lateral_version').decode('utf8'))
  except:
    # if we have trouble reading it, reset it to latest lateral planner
    params.put('dp_lateral_version', "0")
    lat_version = 0
  if lat_version == 1: # 0813
    lateral_planner = DPLateralPlanner(CP)
  elif lat_version == 2: # 0816
    lateral_planner = FPLateralPlanner(CP)
  else:
    lateral_planner = LateralPlanner(CP, debug=debug_mode)

  if sm is None:
    sm = messaging.SubMaster(['carControl', 'carState', 'controlsState', 'radarState', 'modelV2', 'dragonConf', 'lateralPlan', 'liveMapData'],
                             poll=['radarState', 'modelV2'], ignore_avg_freq=['radarState'])

  if pm is None:
    pm = messaging.PubMaster(['longitudinalPlan', 'lateralPlan', 'uiPlan'])

  while True:
    sm.update()

    if sm.updated['modelV2']:
      lateral_planner.update(sm)
      lateral_planner.publish(sm, pm)
      longitudinal_planner.update(sm)
      longitudinal_planner.publish(sm, pm)
      if lat_version == 0:
        publish_ui_plan(sm, pm, lateral_planner, longitudinal_planner)

def main(sm=None, pm=None):
  plannerd_thread(sm, pm)


if __name__ == "__main__":
  main()
