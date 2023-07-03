from cereal import log
from common.conversions import Conversions as CV
from common.realtime import DT_MDL, sec_since_boot

LaneChangeState = log.LateralPlan.LaneChangeState
LaneChangeDirection = log.LateralPlan.LaneChangeDirection

LANE_CHANGE_SPEED_MIN = 20 * CV.MPH_TO_MS
LANE_CHANGE_TIME_MAX = 10.

DESIRES = {
  LaneChangeDirection.none: {
    LaneChangeState.off: log.LateralPlan.Desire.none,
    LaneChangeState.preLaneChange: log.LateralPlan.Desire.none,
    LaneChangeState.laneChangeStarting: log.LateralPlan.Desire.none,
    LaneChangeState.laneChangeFinishing: log.LateralPlan.Desire.none,
  },
  LaneChangeDirection.left: {
    LaneChangeState.off: log.LateralPlan.Desire.none,
    LaneChangeState.preLaneChange: log.LateralPlan.Desire.none,
    LaneChangeState.laneChangeStarting: log.LateralPlan.Desire.laneChangeLeft,
    LaneChangeState.laneChangeFinishing: log.LateralPlan.Desire.laneChangeLeft,
  },
  LaneChangeDirection.right: {
    LaneChangeState.off: log.LateralPlan.Desire.none,
    LaneChangeState.preLaneChange: log.LateralPlan.Desire.none,
    LaneChangeState.laneChangeStarting: log.LateralPlan.Desire.laneChangeRight,
    LaneChangeState.laneChangeFinishing: log.LateralPlan.Desire.laneChangeRight,
  },
}


class DesireHelper:
  def __init__(self):
    self.lane_change_state = LaneChangeState.off
    self.lane_change_direction = LaneChangeDirection.none
    self.lane_change_timer = 0.0
    self.lane_change_ll_prob = 1.0
    self.keep_pulse_timer = 0.0
    self.prev_one_blinker = False
    self.desire = log.LateralPlan.Desire.none

    # dp
    self.dp_lc_auto_done = False
    self.dp_lc_auto_delay_start_sec = None
    self.dp_lateral_mode = 1 # 0 = blinker mode (should we remove?), 1 = assist lane change, 2 = auto lane change
    self.dp_lc_min_mph = LANE_CHANGE_SPEED_MIN
    self.dp_lc_auto_min_mph = LANE_CHANGE_SPEED_MIN + 10
    self.dp_lc_auto_delay = 3 # secs
    self.dp_lateral_road_edge_detected = False

  def update(self, carstate, lateral_active, lane_change_prob, dragonconf, md):
    # dp - sync with dragonConf
    self.dp_lateral_mode = dragonconf.dpLateralMode
    self.dp_lc_min_mph = dragonconf.dpLcMinMph * CV.MPH_TO_MS
    self.dp_lc_auto_min_mph = dragonconf.dpLcAutoMinMph * CV.MPH_TO_MS
    self.dp_lc_auto_min_mph = self.dp_lc_min_mph if self.dp_lc_auto_min_mph < self.dp_lc_min_mph else self.dp_lc_auto_min_mph
    self.dp_lc_auto_delay = dragonconf.dpLcAutoDelay
    self.dp_lateral_road_edge_detected = dragonconf.dpLateralRoadEdgeDetected

    v_ego = carstate.vEgo
    one_blinker = carstate.leftBlinker != carstate.rightBlinker
    below_lane_change_speed = v_ego < self.dp_lc_min_mph
    below_alc_speed = v_ego < self.dp_lc_auto_min_mph

    if not lateral_active or self.lane_change_timer > LANE_CHANGE_TIME_MAX:
      self.lane_change_state = LaneChangeState.off
      self.lane_change_direction = LaneChangeDirection.none
    else:
      blindspot_detected = ((carstate.leftBlindspot and self.lane_change_direction == LaneChangeDirection.left) or
                              (carstate.rightBlindspot and self.lane_change_direction == LaneChangeDirection.right))
      # LaneChangeState.off
      if self.lane_change_state == LaneChangeState.off and one_blinker and not self.prev_one_blinker and not below_lane_change_speed:
        self.lane_change_state = LaneChangeState.preLaneChange
        self.lane_change_ll_prob = 1.0

        self.dp_lc_auto_done = False
        self.dp_lc_auto_delay_start_sec = None

      # LaneChangeState.preLaneChange
      elif self.lane_change_state == LaneChangeState.preLaneChange:
        # Set lane change direction
        self.lane_change_direction = LaneChangeDirection.left if \
          carstate.leftBlinker else LaneChangeDirection.right

        torque_applied = carstate.steeringPressed and \
                         ((carstate.steeringTorque > 0 and self.lane_change_direction == LaneChangeDirection.left) or
                          (carstate.steeringTorque < 0 and self.lane_change_direction == LaneChangeDirection.right))

        if self.dp_lateral_mode == 2:
          if self.dp_lc_auto_delay_start_sec is None:
            self.dp_lc_auto_delay_start_sec = sec_since_boot()
          else:
            if one_blinker and not below_alc_speed and (not self.dp_lc_auto_done) and \
              (sec_since_boot() - self.dp_lc_auto_delay_start_sec >= self.dp_lc_auto_delay):
              torque_applied = True


        #dp
        if self.dp_lateral_road_edge_detected:
          left_road_edge = -md.roadEdges[0].y[0]
          right_road_edge = md.roadEdges[1].y[0]

          road_edge_detected = (((left_road_edge < 3.5) and self.lane_change_direction == LaneChangeDirection.left) or
                                ((right_road_edge < 3.5) and self.lane_change_direction == LaneChangeDirection.right))
        else:
          road_edge_detected = False

        if blindspot_detected:
          self.dp_lc_auto_done = False
          self.dp_lc_auto_delay_start_sec = None
        if not one_blinker or below_lane_change_speed:
          self.lane_change_state = LaneChangeState.off
          self.lane_change_direction = LaneChangeDirection.none
        elif torque_applied and not blindspot_detected and not road_edge_detected:
          self.lane_change_state = LaneChangeState.laneChangeStarting

      # LaneChangeState.laneChangeStarting
      elif self.lane_change_state == LaneChangeState.laneChangeStarting:
        if blindspot_detected:
          self.lane_change_state = LaneChangeState.preLaneChange
          self.lane_change_ll_prob = 1.0
          self.dp_lc_auto_done = False
          self.dp_lc_auto_delay_start_sec = None
        else:
          # fade out over .5s
          self.lane_change_ll_prob = max(self.lane_change_ll_prob - 2 * DT_MDL, 0.0)

          # 98% certainty
          if lane_change_prob < 0.02 and self.lane_change_ll_prob < 0.01:
            self.lane_change_state = LaneChangeState.laneChangeFinishing

      # LaneChangeState.laneChangeFinishing
      elif self.lane_change_state == LaneChangeState.laneChangeFinishing:
        if blindspot_detected:
          self.lane_change_state = LaneChangeState.preLaneChange
          self.lane_change_ll_prob = 1.0
          self.dp_lc_auto_done = False
          self.dp_lc_auto_delay_start_sec = None
        else:
          # fade in laneline over 1s
          self.lane_change_ll_prob = min(self.lane_change_ll_prob + DT_MDL, 1.0)

          if self.lane_change_ll_prob > 0.99:
            self.lane_change_direction = LaneChangeDirection.none
            if one_blinker:
              self.lane_change_state = LaneChangeState.preLaneChange
            else:
              self.lane_change_state = LaneChangeState.off

          self.dp_lc_auto_done = True

    if self.lane_change_state in (LaneChangeState.off, LaneChangeState.preLaneChange):
      self.lane_change_timer = 0.0
    else:
      self.lane_change_timer += DT_MDL

    self.prev_one_blinker = one_blinker

    self.desire = DESIRES[self.lane_change_direction][self.lane_change_state]

    # Send keep pulse once per second during LaneChangeStart.preLaneChange
    if self.lane_change_state in (LaneChangeState.off, LaneChangeState.laneChangeStarting):
      self.keep_pulse_timer = 0.0
    elif self.lane_change_state == LaneChangeState.preLaneChange:
      self.keep_pulse_timer += DT_MDL
      if self.keep_pulse_timer > 1.0:
        self.keep_pulse_timer = 0.0
      elif self.desire in (log.LateralPlan.Desire.keepLeft, log.LateralPlan.Desire.keepRight):
        self.desire = log.LateralPlan.Desire.none
