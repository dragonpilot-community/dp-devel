import copy

from cereal import car
from common.conversions import Conversions as CV
from common.numpy_fast import mean
from common.filter_simple import FirstOrderFilter
from common.realtime import DT_CTRL
from opendbc.can.can_define import CANDefine
from opendbc.can.parser import CANParser
from selfdrive.car.interfaces import CarStateBase
from selfdrive.car.toyota.values import ToyotaFlags, CAR, DBC, STEER_THRESHOLD, NO_STOP_TIMER_CAR, TSS2_CAR, RADAR_ACC_CAR, EPS_SCALE, UNSUPPORTED_DSU_CAR

from common.params import Params, put_nonblocking
import time
from math import floor

# dp
DP_ACCEL_ECO = 0
DP_ACCEL_NORMAL = 1
DP_ACCEL_SPORT = 2

_TRAFFIC_SINGAL_MAP = {
  1: "kph",
  36: "mph",
  65: "No overtake",
  66: "No overtake"
}

SteerControlType = car.CarParams.SteerControlType

# These steering fault definitions seem to be common across LKA (torque) and LTA (angle):
# - high steer rate fault: goes to 21 or 25 for 1 frame, then 9 for 2 seconds
# - lka/lta msg drop out: goes to 9 then 11 for a combined total of 2 seconds, then 3.
#     if using the other control command, goes directly to 3 after 1.5 seconds
# - initializing: LTA can report 0 as long as STEER_TORQUE_SENSOR->STEER_ANGLE_INITIALIZING is 1,
#     and is a catch-all for LKA
TEMP_STEER_FAULTS = (0, 9, 11, 21, 25)


class CarState(CarStateBase):
  def __init__(self, CP):
    super().__init__(CP)
    can_define = CANDefine(DBC[CP.carFingerprint]["pt"])
    self.shifter_values = can_define.dv["GEAR_PACKET"]["GEAR"]
    self.eps_torque_scale = EPS_SCALE[CP.carFingerprint] / 100.
    self.cluster_speed_hyst_gap = CV.KPH_TO_MS / 2.
    self.cluster_min_speed = CV.KPH_TO_MS / 2.

    # On cars with cp.vl["STEER_TORQUE_SENSOR"]["STEER_ANGLE"]
    # the signal is zeroed to where the steering angle is at start.
    # Need to apply an offset as soon as the steering angle measurements are both received
    self.accurate_steer_angle_seen = False
    self.angle_offset = FirstOrderFilter(None, 60.0, DT_CTRL, initialized=False)
    self._init_traffic_signals()

    self.low_speed_lockout = False
    self.acc_type = 1
    self.lkas_hud = {}

    #dp
    self.frame = 0
    self.dp_sig_check = False
    self.dp_sig_sport_on_seen = True
    self.dp_sig_econ_on_seen = True
    self.dp_accel_profile = None
    self.dp_accel_profile_prev = None
    self.dp_accel_profile_init = False
    self.dp_toyota_ap_btn_link = Params().get_bool('dp_toyota_ap_btn_link')
    self.read_distance_lines = 0
    self.read_distance_lines_init = False
    self.distance = 0
    self.dp_toyota_fp_btn_link = Params().get_bool('dp_toyota_fp_btn_link')

    # zss
    self.dp_toyota_zss = Params().get_bool('dp_toyota_zss')
    self.dp_zss_compute = False
    self.dp_zss_cruise_active_last = False
    self.dp_zss_angle_offset = 0.

    # bsm
    self.dp_toyota_debug_bsm = Params().get_bool('dp_toyota_debug_bsm')

    self.left_blindspot = False
    self.left_blindspot_d1 = 0
    self.left_blindspot_d2 = 0
    self.left_blindspot_counter = 0

    self.right_blindspot = False
    self.right_blindspot_d1 = 0
    self.right_blindspot_d2 = 0
    self.right_blindspot_counter = 0

  def update(self, cp, cp_cam):
    ret = car.CarState.new_message()

    ret.doorOpen = any([cp.vl["BODY_CONTROL_STATE"]["DOOR_OPEN_FL"], cp.vl["BODY_CONTROL_STATE"]["DOOR_OPEN_FR"],
                        cp.vl["BODY_CONTROL_STATE"]["DOOR_OPEN_RL"], cp.vl["BODY_CONTROL_STATE"]["DOOR_OPEN_RR"]])
    ret.seatbeltUnlatched = cp.vl["BODY_CONTROL_STATE"]["SEATBELT_DRIVER_UNLATCHED"] != 0
    ret.parkingBrake = cp.vl["BODY_CONTROL_STATE"]["PARKING_BRAKE"] == 1

    ret.brakePressed = cp.vl["BRAKE_MODULE"]["BRAKE_PRESSED"] != 0
    #ret.brakeHoldActive = cp.vl["ESP_CONTROL"]["BRAKE_HOLD_ACTIVE"] == 1
    ret.brakeLightsDEPRECATED = bool(cp.vl["ESP_CONTROL"]['BRAKE_LIGHTS_ACC'] or cp.vl["BRAKE_MODULE"]["BRAKE_PRESSED"] != 0)
    if self.CP.enableGasInterceptor:
      ret.gas = (cp.vl["GAS_SENSOR"]["INTERCEPTOR_GAS"] + cp.vl["GAS_SENSOR"]["INTERCEPTOR_GAS2"]) // 2
      ret.gasPressed = ret.gas > 805
    else:
      # TODO: find a new, common signal
      msg = "GAS_PEDAL_HYBRID" if (self.CP.flags & ToyotaFlags.HYBRID) else "GAS_PEDAL"
      ret.gas = cp.vl[msg]["GAS_PEDAL"]
      ret.gasPressed = cp.vl["PCM_CRUISE"]["GAS_RELEASED"] == 0

    ret.wheelSpeeds = self.get_wheel_speeds(
      cp.vl["WHEEL_SPEEDS"]["WHEEL_SPEED_FL"],
      cp.vl["WHEEL_SPEEDS"]["WHEEL_SPEED_FR"],
      cp.vl["WHEEL_SPEEDS"]["WHEEL_SPEED_RL"],
      cp.vl["WHEEL_SPEEDS"]["WHEEL_SPEED_RR"],
    )
    ret.vEgoRaw = mean([ret.wheelSpeeds.fl, ret.wheelSpeeds.fr, ret.wheelSpeeds.rl, ret.wheelSpeeds.rr])
    ret.vEgo, ret.aEgo = self.update_speed_kf(ret.vEgoRaw)
    ret.vEgoCluster = ret.vEgo * 1.015  # minimum of all the cars

    ret.standstill = ret.vEgoRaw == 0

    ret.steeringAngleDeg = cp.vl["STEER_ANGLE_SENSOR"]["STEER_ANGLE"] + cp.vl["STEER_ANGLE_SENSOR"]["STEER_FRACTION"]
    torque_sensor_angle_deg = cp.vl["STEER_TORQUE_SENSOR"]["STEER_ANGLE"]

    # On some cars, the angle measurement is non-zero while initializing
    if abs(torque_sensor_angle_deg) > 1e-3 and not bool(cp.vl["STEER_TORQUE_SENSOR"]["STEER_ANGLE_INITIALIZING"]):
      self.accurate_steer_angle_seen = True

    if self.accurate_steer_angle_seen:
      # Offset seems to be invalid for large steering angles
      if abs(ret.steeringAngleDeg) < 90 and cp.can_valid:
        self.angle_offset.update(torque_sensor_angle_deg - ret.steeringAngleDeg)

      if self.angle_offset.initialized:
        ret.steeringAngleOffsetDeg = self.angle_offset.x
        ret.steeringAngleDeg = torque_sensor_angle_deg - self.angle_offset.x

    # dp - toyota zss
    if self.dp_toyota_zss:
      zorro_steer = cp.vl["SECONDARY_STEER_ANGLE"]["ZORRO_STEER"]
      # only compute zss offset when acc is active
      if bool(cp.vl["PCM_CRUISE"]["CRUISE_ACTIVE"]) and not self.dp_zss_cruise_active_last:
        self.dp_zss_compute = True # cruise was just activated, so allow offset to be recomputed
      self.dp_zss_cruise_active_last = bool(cp.vl["PCM_CRUISE"]["CRUISE_ACTIVE"])

      # compute zss offset
      if self.dp_zss_compute:
        if abs(ret.steeringAngleDeg) > 1e-3 and abs(zorro_steer) > 1e-3:
          self.dp_toyota_zss = False
          self.dp_zss_angle_offset = zorro_steer - ret.steeringAngleDeg
      # apply offset
      ret.steeringAngleDeg = zorro_steer - self.dp_zss_angle_offset

    ret.steeringRateDeg = cp.vl["STEER_ANGLE_SENSOR"]["STEER_RATE"]

    can_gear = int(cp.vl["GEAR_PACKET"]["GEAR"])
    ret.gearShifter = self.parse_gear_shifter(self.shifter_values.get(can_gear, None))

    #dp: Thank you Arne (acceleration)
    if self.dp_toyota_ap_btn_link:
      sport_on_sig = 'SPORT_ON_2' if self.CP.carFingerprint in (CAR.RAV4_TSS2, CAR.LEXUS_ES_TSS2, CAR.HIGHLANDER_TSS2) else 'SPORT_ON'
      # check signal once
      if not self.dp_sig_check:
        self.dp_sig_check = True
        # sport on
        try:
          sport_on = cp.vl["GEAR_PACKET"][sport_on_sig]
        except KeyError:
          sport_on = 0
          self.dp_sig_sport_on_seen = False
        # econ on
        try:
          econ_on = cp.vl["GEAR_PACKET"]['ECON_ON']
        except KeyError:
          econ_on = 0
          self.dp_sig_econ_on_seen = False
      else:
        sport_on = cp.vl["GEAR_PACKET"][sport_on_sig] if self.dp_sig_sport_on_seen else 0
        econ_on = cp.vl["GEAR_PACKET"]['ECON_ON'] if self.dp_sig_econ_on_seen else 0

      if sport_on == 0 and econ_on == 0:
        self.dp_accel_profile = DP_ACCEL_NORMAL
      elif sport_on == 1:
        self.dp_accel_profile = DP_ACCEL_SPORT
      elif econ_on == 1:
        self.dp_accel_profile = DP_ACCEL_ECO

      # if init is false, we sync profile with whatever mode we have on car
      if not self.dp_accel_profile_init or self.dp_accel_profile != self.dp_accel_profile_prev:
        put_nonblocking('dp_accel_profile', str(self.dp_accel_profile))
        put_nonblocking('dp_last_modified',str(floor(time.time())))
        self.dp_accel_profile_init = True
      self.dp_accel_profile_prev = self.dp_accel_profile

    # distance button

    #dp: Thank you Arne (distance button)
    if self.dp_toyota_fp_btn_link:
      if not self.read_distance_lines_init or self.read_distance_lines != cp.vl["PCM_CRUISE_SM"]['DISTANCE_LINES']:
        self.read_distance_lines_init = True
        self.read_distance_lines = cp.vl["PCM_CRUISE_SM"]['DISTANCE_LINES']
        put_nonblocking('dp_following_profile', str(int(max(self.read_distance_lines - 1, 0)))) # Skipping one profile toyota mid is weird.
        put_nonblocking('dp_last_modified',str(floor(time.time())))

    if self.CP.carFingerprint in (TSS2_CAR - RADAR_ACC_CAR):
      # KRKeegan - Add support for toyota distance button
      self.distance = 1 if cp_cam.vl["ACC_CONTROL"]["DISTANCE"] == 1 else 0
      ret.distanceLines = cp.vl["PCM_CRUISE_SM"]["DISTANCE_LINES"]

    if self.CP.carFingerprint in RADAR_ACC_CAR:
      # KRKeegan - Add support for toyota distance button these cars have the acc_control on car can
      self.distance = 1 if cp.vl["ACC_CONTROL"]["DISTANCE"] == 1 else 0
      ret.distanceLines = cp.vl["PCM_CRUISE_SM"]["DISTANCE_LINES"]
    #dp
    ret.engineRpm = cp.vl["ENGINE_RPM"]['RPM']

    ret.leftBlinker = cp.vl["BLINKERS_STATE"]["TURN_SIGNALS"] == 1
    ret.rightBlinker = cp.vl["BLINKERS_STATE"]["TURN_SIGNALS"] == 2

    ret.steeringTorque = cp.vl["STEER_TORQUE_SENSOR"]["STEER_TORQUE_DRIVER"]
    ret.steeringTorqueEps = cp.vl["STEER_TORQUE_SENSOR"]["STEER_TORQUE_EPS"] * self.eps_torque_scale
    # we could use the override bit from dbc, but it's triggered at too high torque values
    ret.steeringPressed = abs(ret.steeringTorque) > STEER_THRESHOLD

    # Check EPS LKA/LTA fault status
    ret.steerFaultTemporary = cp.vl["EPS_STATUS"]["LKA_STATE"] in TEMP_STEER_FAULTS
    # 3 is a fault from the lka command message not being received by the EPS (recoverable)
    # 17 is a fault from a prolonged high torque delta between cmd and user (permanent)
    ret.steerFaultPermanent = cp.vl["EPS_STATUS"]["LKA_STATE"] in (3, 17)

    if self.CP.steerControlType == SteerControlType.angle:
      ret.steerFaultTemporary = ret.steerFaultTemporary or cp.vl["EPS_STATUS"]["LTA_STATE"] in TEMP_STEER_FAULTS
      ret.steerFaultPermanent = ret.steerFaultPermanent or cp.vl["EPS_STATUS"]["LTA_STATE"] in (3,)

    if self.CP.carFingerprint in UNSUPPORTED_DSU_CAR:
      # TODO: find the bit likely in DSU_CRUISE that describes an ACC fault. one may also exist in CLUTCH
      ret.cruiseState.available = cp.vl["DSU_CRUISE"]["MAIN_ON"] != 0
      ret.cruiseState.speed = cp.vl["DSU_CRUISE"]["SET_SPEED"] * CV.KPH_TO_MS
      cluster_set_speed = cp.vl["PCM_CRUISE_ALT"]["UI_SET_SPEED"]
    else:
      ret.accFaulted = cp.vl["PCM_CRUISE_2"]["ACC_FAULTED"] != 0
      ret.cruiseState.available = cp.vl["PCM_CRUISE_2"]["MAIN_ON"] != 0
      ret.cruiseState.speed = cp.vl["PCM_CRUISE_2"]["SET_SPEED"] * CV.KPH_TO_MS
      cluster_set_speed = cp.vl["PCM_CRUISE_SM"]["UI_SET_SPEED"]

    # UI_SET_SPEED is always non-zero when main is on, hide until first enable
    if ret.cruiseState.speed != 0:
      is_metric = cp.vl["BODY_CONTROL_STATE_2"]["UNITS"] in (1, 2)
      conversion_factor = CV.KPH_TO_MS if is_metric else CV.MPH_TO_MS
      ret.cruiseState.speedCluster = cluster_set_speed * conversion_factor

    cp_acc = cp_cam if self.CP.carFingerprint in (TSS2_CAR - RADAR_ACC_CAR) else cp

    if self.CP.carFingerprint in (TSS2_CAR | RADAR_ACC_CAR):
      if not (self.CP.flags & ToyotaFlags.SMART_DSU.value):
        self.acc_type = cp_acc.vl["ACC_CONTROL"]["ACC_TYPE"]
      ret.stockFcw = bool(cp_acc.vl["ACC_HUD"]["FCW"])

    # some TSS2 cars have low speed lockout permanently set, so ignore on those cars
    # these cars are identified by an ACC_TYPE value of 2.
    # TODO: it is possible to avoid the lockout and gain stop and go if you
    # send your own ACC_CONTROL msg on startup with ACC_TYPE set to 1
    if (self.CP.carFingerprint not in TSS2_CAR and self.CP.carFingerprint not in UNSUPPORTED_DSU_CAR) or \
       (self.CP.carFingerprint in TSS2_CAR and self.acc_type == 1):
      self.low_speed_lockout = cp.vl["PCM_CRUISE_2"]["LOW_SPEED_LOCKOUT"] == 2

    self.pcm_acc_status = cp.vl["PCM_CRUISE"]["CRUISE_STATE"]
    if self.CP.carFingerprint not in (NO_STOP_TIMER_CAR - TSS2_CAR):
      # ignore standstill state in certain vehicles, since pcm allows to restart with just an acceleration request
      ret.cruiseState.standstill = self.pcm_acc_status == 7
    ret.cruiseState.enabled = bool(cp.vl["PCM_CRUISE"]["CRUISE_ACTIVE"])
    ret.cruiseState.nonAdaptive = cp.vl["PCM_CRUISE"]["CRUISE_STATE"] in (1, 2, 3, 4, 5, 6)

    ret.genericToggle = bool(cp.vl["LIGHT_STALK"]["AUTO_HIGH_BEAM"])
    ret.espDisabled = cp.vl["ESP_CONTROL"]["TC_DISABLED"] != 0

    if not self.CP.enableDsu:
      ret.stockAeb = bool(cp_acc.vl["PRE_COLLISION"]["PRECOLLISION_ACTIVE"] and cp_acc.vl["PRE_COLLISION"]["FORCE"] < -1e-5)

    if self.CP.enableBsm:
      ret.leftBlindspot = (cp.vl["BSM"]["L_ADJACENT"] == 1) or (cp.vl["BSM"]["L_APPROACHING"] == 1)
      ret.rightBlindspot = (cp.vl["BSM"]["R_ADJACENT"] == 1) or (cp.vl["BSM"]["R_APPROACHING"] == 1)

    # Enable blindspot debug mode once (@arne182)
    # let's keep all the commented out code for easy debug purpose for future.
    if self.dp_toyota_debug_bsm and self.frame > 1999: #self.CP.carFingerprint == CAR.PRIUS_TSS2: #not (self.CP.carFingerprint in TSS2_CAR or self.CP.carFingerprint == CAR.CAMRY or self.CP.carFingerprint == CAR.CAMRYH):
        distance_1 = cp.vl["DEBUG"].get('BLINDSPOTD1')
        distance_2 = cp.vl["DEBUG"].get('BLINDSPOTD2')
        side = cp.vl["DEBUG"].get('BLINDSPOTSIDE')

        if distance_1 is not None and distance_2 is not None and side is not None:
          if side == 65: # Left blind spot
            if distance_1 != self.left_blindspot_d1:
                self.left_blindspot_d1 = distance_1
                self.left_blindspot_counter = 100
            if distance_2 != self.left_blindspot_d2:
                self.left_blindspot_d2 = distance_2
                self.left_blindspot_counter = 100
            if self.left_blindspot_d1 > 10 or self.left_blindspot_d2 > 10:
                self.left_blindspot = True
          elif side == 66: # Right blind spot
            if distance_1 != self.right_blindspot_d1:
                self.right_blindspot_d1 = distance_1
                self.right_blindspot_counter = 100
            if distance_2 != self.right_blindspot_d2:
                self.right_blindspot_d2 = distance_2
                self.right_blindspot_counter = 100
            if self.right_blindspot_d1 > 10 or self.right_blindspot_d2 > 10:
                self.right_blindspot = True

          if self.left_blindspot_counter > 0:
            self.left_blindspot_counter -= 2
          else:
            self.left_blindspot = False
            self.left_blindspot_d1 = 0
            self.left_blindspot_d2 = 0

          if self.right_blindspot_counter > 0:
            self.right_blindspot_counter -= 2
          else:
            self.right_blindspot = False
            self.right_blindspot_d1 = 0
            self.right_blindspot_d2 = 0

          ret.leftBlindspot = self.left_blindspot
          ret.rightBlindspot = self.right_blindspot


    if self.CP.carFingerprint != CAR.PRIUS_V:
      self.lkas_hud = copy.copy(cp_cam.vl["LKAS_HUD"])

    self._update_traffic_signals(cp_cam)
    ret.cruiseState.speedLimit = self._calculate_speed_limit()

    self.frame += 1
    return ret

  def _init_traffic_signals(self):
    self._tsgn1 = None
    self._spdval1 = None
    self._splsgn1 = None
    self._tsgn2 = None
    self._splsgn2 = None
    self._tsgn3 = None
    self._splsgn3 = None
    self._tsgn4 = None
    self._splsgn4 = None

  def _update_traffic_signals(self, cp_cam):
    # Print out car signals for traffic signal detection
    tsgn1 = cp_cam.vl["RSA1"]['TSGN1']
    spdval1 = cp_cam.vl["RSA1"]['SPDVAL1']
    splsgn1 = cp_cam.vl["RSA1"]['SPLSGN1']
    tsgn2 = cp_cam.vl["RSA1"]['TSGN2']
    splsgn2 = cp_cam.vl["RSA1"]['SPLSGN2']
    tsgn3 = cp_cam.vl["RSA2"]['TSGN3']
    splsgn3 = cp_cam.vl["RSA2"]['SPLSGN3']
    tsgn4 = cp_cam.vl["RSA2"]['TSGN4']
    splsgn4 = cp_cam.vl["RSA2"]['SPLSGN4']

    has_changed = tsgn1 != self._tsgn1 \
      or spdval1 != self._spdval1 \
      or splsgn1 != self._splsgn1 \
      or tsgn2 != self._tsgn2 \
      or splsgn2 != self._splsgn2 \
      or tsgn3 != self._tsgn3 \
      or splsgn3 != self._splsgn3 \
      or tsgn4 != self._tsgn4 \
      or splsgn4 != self._splsgn4

    self._tsgn1 = tsgn1
    self._spdval1 = spdval1
    self._splsgn1 = splsgn1
    self._tsgn2 = tsgn2
    self._splsgn2 = splsgn2
    self._tsgn3 = tsgn3
    self._splsgn3 = splsgn3
    self._tsgn4 = tsgn4
    self._splsgn4 = splsgn4

    if not has_changed:
      return

    print('---- TRAFFIC SIGNAL UPDATE -----')
    if tsgn1 is not None and tsgn1 != 0:
      print(f'TSGN1: {self._traffic_signal_description(tsgn1)}')
    if spdval1 is not None and spdval1 != 0:
      print(f'SPDVAL1: {spdval1}')
    if splsgn1 is not None and splsgn1 != 0:
      print(f'SPLSGN1: {splsgn1}')
    if tsgn2 is not None and tsgn2 != 0:
      print(f'TSGN2: {self._traffic_signal_description(tsgn2)}')
    if splsgn2 is not None and splsgn2 != 0:
      print(f'SPLSGN2: {splsgn2}')
    if tsgn3 is not None and tsgn3 != 0:
      print(f'TSGN3: {self._traffic_signal_description(tsgn3)}')
    if splsgn3 is not None and splsgn3 != 0:
      print(f'SPLSGN3: {splsgn3}')
    if tsgn4 is not None and tsgn4 != 0:
      print(f'TSGN4: {self._traffic_signal_description(tsgn4)}')
    if splsgn4 is not None and splsgn4 != 0:
      print(f'SPLSGN4: {splsgn4}')
    print('------------------------')

  def _traffic_signal_description(self, tsgn):
    desc = _TRAFFIC_SINGAL_MAP.get(int(tsgn))
    return f'{tsgn}: {desc}' if desc is not None else f'{tsgn}'

  def _calculate_speed_limit(self):
    if self._tsgn1 == 1:
      return self._spdval1 * CV.KPH_TO_MS
    if self._tsgn1 == 36:
      return self._spdval1 * CV.MPH_TO_MS
    return 0

  @staticmethod
  def get_can_parser(CP):
    signals = [
      # sig_name, sig_address
      ("STEER_ANGLE", "STEER_ANGLE_SENSOR"),
      ("GEAR", "GEAR_PACKET"),
      ("BRAKE_PRESSED", "BRAKE_MODULE"),
      ("WHEEL_SPEED_FL", "WHEEL_SPEEDS"),
      ("WHEEL_SPEED_FR", "WHEEL_SPEEDS"),
      ("WHEEL_SPEED_RL", "WHEEL_SPEEDS"),
      ("WHEEL_SPEED_RR", "WHEEL_SPEEDS"),
      ("DOOR_OPEN_FL", "BODY_CONTROL_STATE"),
      ("DOOR_OPEN_FR", "BODY_CONTROL_STATE"),
      ("DOOR_OPEN_RL", "BODY_CONTROL_STATE"),
      ("DOOR_OPEN_RR", "BODY_CONTROL_STATE"),
      ("SEATBELT_DRIVER_UNLATCHED", "BODY_CONTROL_STATE"),
      ("PARKING_BRAKE", "BODY_CONTROL_STATE"),
      ("UNITS", "BODY_CONTROL_STATE_2"),
      ("TC_DISABLED", "ESP_CONTROL"),
      #("BRAKE_HOLD_ACTIVE", "ESP_CONTROL"),
      ("STEER_FRACTION", "STEER_ANGLE_SENSOR"),
      ("STEER_RATE", "STEER_ANGLE_SENSOR"),
      ("CRUISE_ACTIVE", "PCM_CRUISE"),
      ("CRUISE_STATE", "PCM_CRUISE"),
      ("GAS_RELEASED", "PCM_CRUISE"),
      ("UI_SET_SPEED", "PCM_CRUISE_SM"),
      ("STEER_TORQUE_DRIVER", "STEER_TORQUE_SENSOR"),
      ("STEER_TORQUE_EPS", "STEER_TORQUE_SENSOR"),
      ("STEER_ANGLE", "STEER_TORQUE_SENSOR"),
      ("STEER_ANGLE_INITIALIZING", "STEER_TORQUE_SENSOR"),
      ("TURN_SIGNALS", "BLINKERS_STATE"),
      ("LKA_STATE", "EPS_STATUS"),
      ("AUTO_HIGH_BEAM", "LIGHT_STALK"),
      #dp
      ("SPORT_ON", "GEAR_PACKET"),
      ("ECON_ON", "GEAR_PACKET"),
      ("RPM", "ENGINE_RPM"),
      ("BRAKE_LIGHTS_ACC", "ESP_CONTROL"),
      ("DISTANCE_LINES", "PCM_CRUISE_SM"),
    ]

    # Check LTA state if using LTA angle control
    if CP.steerControlType == SteerControlType.angle:
      signals.append(("LTA_STATE", "EPS_STATUS"))

    checks = [
      ("GEAR_PACKET", 1),
      ("LIGHT_STALK", 1),
      ("BLINKERS_STATE", 0.15),
      ("BODY_CONTROL_STATE", 3),
      ("BODY_CONTROL_STATE_2", 2),
      ("ESP_CONTROL", 3),
      ("EPS_STATUS", 25),
      ("BRAKE_MODULE", 40),
      ("WHEEL_SPEEDS", 80),
      ("STEER_ANGLE_SENSOR", 80),
      ("PCM_CRUISE", 33),
      ("PCM_CRUISE_SM", 1),
      ("STEER_TORQUE_SENSOR", 50),
      #dp
      ("ENGINE_RPM", 100),
    ]

    if CP.flags & ToyotaFlags.HYBRID:
      signals.append(("GAS_PEDAL", "GAS_PEDAL_HYBRID"))
      checks.append(("GAS_PEDAL_HYBRID", 33))
    else:
      signals.append(("GAS_PEDAL", "GAS_PEDAL"))
      checks.append(("GAS_PEDAL", 33))
    #dp acceleration
    if CP.carFingerprint in (CAR.RAV4_TSS2, CAR.LEXUS_ES_TSS2, CAR.HIGHLANDER_TSS2):
      signals.append(("SPORT_ON_2", "GEAR_PACKET"))

    if CP.carFingerprint in (CAR.ALPHARD_TSS2, CAR.ALPHARDH_TSS2, CAR.AVALON_TSS2, CAR.AVALONH_TSS2, CAR.CAMRY_TSS2, CAR.CAMRYH_TSS2, CAR.CHR_TSS2, CAR.COROLLA_TSS2, CAR.COROLLAH_TSS2, CAR.HIGHLANDER_TSS2, CAR.HIGHLANDERH_TSS2, CAR.PRIUS_TSS2, CAR.RAV4H_TSS2, CAR.MIRAI, CAR.LEXUS_ES_TSS2, CAR.LEXUS_ESH_TSS2, CAR.LEXUS_NX_TSS2, CAR.LEXUS_NXH_TSS2, CAR.LEXUS_RX_TSS2, CAR.LEXUS_RXH_TSS2, CAR.CHRH):
      signals.append(("SPORT_ON", "GEAR_PACKET"))
      signals.append(("ECON_ON", "GEAR_PACKET"))

    if CP.flags & ToyotaFlags.SMART_DSU:
      signals.append(("FD_BUTTON", "SDSU"))
      checks.append(("SDSU", 0))

    if CP.carFingerprint in UNSUPPORTED_DSU_CAR:
      signals.append(("MAIN_ON", "DSU_CRUISE"))
      signals.append(("SET_SPEED", "DSU_CRUISE"))
      signals.append(("UI_SET_SPEED", "PCM_CRUISE_ALT"))
      checks.append(("DSU_CRUISE", 5))
      checks.append(("PCM_CRUISE_ALT", 1))
    else:
      signals.append(("MAIN_ON", "PCM_CRUISE_2"))
      signals.append(("SET_SPEED", "PCM_CRUISE_2"))
      signals.append(("ACC_FAULTED", "PCM_CRUISE_2"))
      signals.append(("LOW_SPEED_LOCKOUT", "PCM_CRUISE_2"))
      checks.append(("PCM_CRUISE_2", 33))

    # add gas interceptor reading if we are using it
    if CP.enableGasInterceptor:
      signals.append(("INTERCEPTOR_GAS", "GAS_SENSOR"))
      signals.append(("INTERCEPTOR_GAS2", "GAS_SENSOR"))
      checks.append(("GAS_SENSOR", 50))


    dp_toyota_debug_bsm = Params().get_bool('dp_toyota_debug_bsm')

    if CP.enableBsm:
      signals += [
        ("L_ADJACENT", "BSM"),
        ("L_APPROACHING", "BSM"),
        ("R_ADJACENT", "BSM"),
        ("R_APPROACHING", "BSM"),
      ]
      checks.append(("BSM", 1))

    if dp_toyota_debug_bsm:
        signals +=[
         ("BLINDSPOT", "DEBUG"),
         ("BLINDSPOTSIDE", "DEBUG"),
         ("BLINDSPOTD1", "DEBUG"),
         ("BLINDSPOTD2", "DEBUG"),
        ]

    if CP.carFingerprint in RADAR_ACC_CAR:
      if not CP.flags & ToyotaFlags.SMART_DSU.value:
        signals += [
          ("ACC_TYPE", "ACC_CONTROL"),
        ]
        checks += [
          ("ACC_CONTROL", 33),
        ]
      signals += [
        ("FCW", "ACC_HUD"),
      ]
      checks += [
        ("ACC_HUD", 1),
      ]

    if CP.carFingerprint not in (TSS2_CAR - RADAR_ACC_CAR) and not CP.enableDsu:
      signals += [
        ("FORCE", "PRE_COLLISION"),
        ("PRECOLLISION_ACTIVE", "PRE_COLLISION"),
      ]
      checks += [
        ("PRE_COLLISION", 33),
      ]

    # dp - add zss signal check
    if Params().get_bool('dp_toyota_zss'):
      signals += [("ZORRO_STEER", "SECONDARY_STEER_ANGLE", 0)]
      checks += [("SECONDARY_STEER_ANGLE", 0)]

    return CANParser(DBC[CP.carFingerprint]["pt"], signals, checks, 0)

  @staticmethod
  def get_cam_can_parser(CP):
    # Include traffic signal, single
    signals = [
      ("TSGN1", "RSA1", 0),
      ("SPDVAL1", "RSA1", 0),
      ("SPLSGN1", "RSA1", 0),
      ("TSGN2", "RSA1", 0),
      ("SPLSGN2", "RSA1", 0),
      ("TSGN3", "RSA2", 0),
      ("SPLSGN3", "RSA2", 0),
      ("TSGN4", "RSA2", 0),
      ("SPLSGN4", "RSA2", 0),
    ]

    # use steering message to check if panda is connected to frc
    checks = [
      ("RSA1", 0),
      ("RSA2", 0),
    ]


    if CP.carFingerprint != CAR.PRIUS_V:
      signals += [
        ("LANE_SWAY_FLD", "LKAS_HUD"),
        ("LANE_SWAY_BUZZER", "LKAS_HUD"),
        ("LANE_SWAY_WARNING", "LKAS_HUD"),
        ("LANE_SWAY_SENSITIVITY", "LKAS_HUD"),
        ("LANE_SWAY_TOGGLE", "LKAS_HUD"),
      ]
      checks += [
        ("LKAS_HUD", 1),
      ]

    if CP.carFingerprint in (TSS2_CAR - RADAR_ACC_CAR):
      signals += [
        ("PRECOLLISION_ACTIVE", "PRE_COLLISION"),
        ("FORCE", "PRE_COLLISION"),
        ("ACC_TYPE", "ACC_CONTROL"),
        ("FCW", "ACC_HUD"),
        #dp
        ("DISTANCE_LINES", "PCM_CRUISE_SM"),
        ("DISTANCE", "ACC_CONTROL"),
      ]
      checks += [
        ("PRE_COLLISION", 33),
        ("ACC_CONTROL", 33),
        ("ACC_HUD", 1),
        ("PCM_CRUISE_SM", 0),
      ]

    return CANParser(DBC[CP.carFingerprint]["pt"], signals, checks, 2)
