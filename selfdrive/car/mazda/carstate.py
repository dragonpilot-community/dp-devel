from cereal import car
from common.conversions import Conversions as CV
from opendbc.can.can_define import CANDefine
from opendbc.can.parser import CANParser
from selfdrive.car.interfaces import CarStateBase
from selfdrive.car.mazda.values import DBC, LKAS_LIMITS, GEN1, TI_STATE, CAR

class CarState(CarStateBase):
  def __init__(self, CP):
    super().__init__(CP)

    can_define = CANDefine(DBC[CP.carFingerprint]["pt"])
    self.shifter_values = can_define.dv["GEAR"]["GEAR"]

    self.crz_btns_counter = 0
    self.acc_active_last = False
    self.low_speed_alert = False
    self.lkas_allowed_speed = False
    self.lkas_disabled = False

    self.ti_ramp_down = False
    self.ti_version = 1
    self.ti_state = TI_STATE.RUN
    self.ti_violation = 0
    self.ti_error = 0
    self.ti_lkas_allowed = False

  def update(self, cp, cp_cam, cp_body):

    ret = car.CarState.new_message()
    ret.wheelSpeeds = self.get_wheel_speeds(
      cp.vl["WHEEL_SPEEDS"]["FL"],
      cp.vl["WHEEL_SPEEDS"]["FR"],
      cp.vl["WHEEL_SPEEDS"]["RL"],
      cp.vl["WHEEL_SPEEDS"]["RR"],
    )
    ret.vEgoRaw = (ret.wheelSpeeds.fl + ret.wheelSpeeds.fr + ret.wheelSpeeds.rl + ret.wheelSpeeds.rr) / 4.
    ret.vEgo, ret.aEgo = self.update_speed_kf(ret.vEgoRaw)

    # Match panda speed reading
    speed_kph = cp.vl["ENGINE_DATA"]["SPEED"]
    ret.standstill = speed_kph < .1

    can_gear = int(cp.vl["GEAR"]["GEAR"])
    ret.gearShifter = self.parse_gear_shifter(self.shifter_values.get(can_gear, None))

    ret.genericToggle = bool(cp.vl["BLINK_INFO"]["HIGH_BEAMS"])
    ret.leftBlindspot = cp.vl["BSM"]["LEFT_BS1"] == 1
    ret.rightBlindspot = cp.vl["BSM"]["RIGHT_BS1"] == 1
    ret.leftBlinker, ret.rightBlinker = self.update_blinker_from_lamp(40, cp.vl["BLINK_INFO"]["LEFT_BLINK"] == 1,
                                                                      cp.vl["BLINK_INFO"]["RIGHT_BLINK"] == 1)

    if self.CP.enableTorqueInterceptor:
      ret.steeringTorque = cp_body.vl["TI_FEEDBACK"]["TI_TORQUE_SENSOR"]

      self.ti_version = cp_body.vl["TI_FEEDBACK"]["VERSION_NUMBER"]
      self.ti_state = cp_body.vl["TI_FEEDBACK"]["STATE"] # DISCOVER = 0, OFF = 1, DRIVER_OVER = 2, RUN=3
      self.ti_violation = cp_body.vl["TI_FEEDBACK"]["VIOL"] # 0 = no violation
      self.ti_error = cp_body.vl["TI_FEEDBACK"]["ERROR"] # 0 = no error
      if self.ti_version > 1:
        self.ti_ramp_down = (cp_body.vl["TI_FEEDBACK"]["RAMP_DOWN"] == 1)

      ret.steeringPressed = abs(ret.steeringTorque) > LKAS_LIMITS.TI_STEER_THRESHOLD
      self.ti_lkas_allowed = not self.ti_ramp_down and self.ti_state == TI_STATE.RUN
    else:
      ret.steeringTorque = cp.vl["STEER_TORQUE"]["STEER_TORQUE_SENSOR"]
      ret.steeringPressed = abs(ret.steeringTorque) > LKAS_LIMITS.STEER_THRESHOLD

    ret.steeringAngleDeg = cp.vl["STEER"]["STEER_ANGLE"]      

    ret.steeringTorqueEps = cp.vl["STEER_TORQUE"]["STEER_TORQUE_MOTOR"]
    ret.steeringRateDeg = cp.vl["STEER_RATE"]["STEER_ANGLE_RATE"]

    # TODO: this should be from 0 - 1.
    ret.brakePressed = cp.vl["PEDALS"]["BRAKE_ON"] == 1
    ret.brake = cp.vl["BRAKE"]["BRAKE_PRESSURE"]

    ret.seatbeltUnlatched = cp.vl["SEATBELT"]["DRIVER_SEATBELT"] == 0
    ret.doorOpen = any([cp.vl["DOORS"]["FL"], cp.vl["DOORS"]["FR"],
                        cp.vl["DOORS"]["BL"], cp.vl["DOORS"]["BR"]])

    # TODO: this should be from 0 - 1.
    ret.gas = cp.vl["ENGINE_DATA"]["PEDAL_GAS"]
    ret.gasPressed = ret.gas > 0

    # Either due to low speed or hands off
    lkas_blocked = cp.vl["STEER_RATE"]["LKAS_BLOCK"] == 1

    if self.CP.minSteerSpeed > 0:
      # LKAS is enabled at 52kph going up and disabled at 45kph going down
      # wait for LKAS_BLOCK signal to clear when going up since it lags behind the speed sometimes
      if speed_kph > LKAS_LIMITS.ENABLE_SPEED and not lkas_blocked:
        self.lkas_allowed_speed = True
      elif speed_kph < LKAS_LIMITS.DISABLE_SPEED:
        self.lkas_allowed_speed = False
    else:
      self.lkas_allowed_speed = True

    # TODO: the signal used for available seems to be the adaptive cruise signal, instead of the main on
    #       it should be used for carState.cruiseState.nonAdaptive instead
    ret.cruiseState.available = cp.vl["CRZ_CTRL"]["CRZ_AVAILABLE"] == 1
    ret.cruiseState.enabled = cp.vl["CRZ_CTRL"]["CRZ_ACTIVE"] == 1
    ret.cruiseState.standstill = cp.vl["PEDALS"]["STANDSTILL"] == 1
    ret.cruiseState.speed = cp.vl["CRZ_EVENTS"]["CRZ_SPEED"] * CV.KPH_TO_MS

    # On if no driver torque the last 5 seconds
    if self.CP.carFingerprint not in (CAR.CX5_2022, CAR.CX9_2021): 
      ret.steerFaultTemporary = cp.vl["STEER_RATE"]["HANDS_OFF_5_SECONDS"] == 1
    else:
      ret.steerFaultTemporary = False

    self.acc_active_last = ret.cruiseState.enabled

    self.crz_btns_counter = cp.vl["CRZ_BTNS"]["CTR"]

    # camera signals
    self.lkas_disabled = cp_cam.vl["CAM_LANEINFO"]["LANE_LINES"] == 0
    self.cam_lkas = cp_cam.vl["CAM_LKAS"]
    self.cam_laneinfo = cp_cam.vl["CAM_LANEINFO"]
    ret.steerFaultPermanent = cp_cam.vl["CAM_LKAS"]["ERR_BIT_1"] == 1

    return ret

  @staticmethod
  def get_can_parser(CP):
    signals = [
      # sig_name, sig_address
      ("LEFT_BLINK", "BLINK_INFO"),
      ("RIGHT_BLINK", "BLINK_INFO"),
      ("HIGH_BEAMS", "BLINK_INFO"),
      ("STEER_ANGLE", "STEER"),
      ("STEER_ANGLE_RATE", "STEER_RATE"),
      ("STEER_TORQUE_SENSOR", "STEER_TORQUE"),
      ("STEER_TORQUE_MOTOR", "STEER_TORQUE"),
      ("FL", "WHEEL_SPEEDS"),
      ("FR", "WHEEL_SPEEDS"),
      ("RL", "WHEEL_SPEEDS"),
      ("RR", "WHEEL_SPEEDS"),
    ]
    checks = [
      # sig_address, frequency
      ("BLINK_INFO", 10),
      ("STEER", 67),
      ("STEER_RATE", 83),
      ("STEER_TORQUE", 83),
      ("WHEEL_SPEEDS", 100),
    ]
    if CP.carFingerprint in GEN1:
      signals += [
        ("LKAS_BLOCK", "STEER_RATE"),
        ("LKAS_TRACK_STATE", "STEER_RATE"),
        ("HANDS_OFF_5_SECONDS", "STEER_RATE"),
        ("CRZ_ACTIVE", "CRZ_CTRL"),
        ("CRZ_AVAILABLE", "CRZ_CTRL"),
        ("CRZ_SPEED", "CRZ_EVENTS"),
        ("STANDSTILL", "PEDALS"),
        ("BRAKE_ON", "PEDALS"),
        ("BRAKE_PRESSURE", "BRAKE"),
        ("GEAR", "GEAR"),
        ("DRIVER_SEATBELT", "SEATBELT"),
        ("FL", "DOORS"),
        ("FR", "DOORS"),
        ("BL", "DOORS"),
        ("BR", "DOORS"),
        ("PEDAL_GAS", "ENGINE_DATA"),
        ("SPEED", "ENGINE_DATA"),
        ("CTR", "CRZ_BTNS"),
        ("LEFT_BS1", "BSM"),
        ("RIGHT_BS1", "BSM"),
      ]

      checks += [
        ("ENGINE_DATA", 100),
        ("CRZ_CTRL", 50),
        ("CRZ_EVENTS", 50),
        ("CRZ_BTNS", 10),
        ("PEDALS", 50),
        ("BRAKE", 50),
        ("SEATBELT", 10),
        ("DOORS", 10),
        ("GEAR", 20),
        ("BSM", 10),
      ]
    # get real driver torque if we are using a torque interceptor
    if CP.enableTorqueInterceptor:
      signals += [
        ("TI_TORQUE_SENSOR", "TI_FEEDBACK", 0),
        ("CHKSUM", "TI_FEEDBACK", 0),
        ("VERSION_NUMBER", "TI_FEEDBACK", 0),
        ("STATE", "TI_FEEDBACK", 0),
        ("VIOL", "TI_FEEDBACK", 0),
        ("ERROR", "TI_FEEDBACK", 0),
        ("RAMP_DOWN", "TI_FEEDBACK", 0),
      ]

      checks += [
        ("TI_FEEDBACK", 100),
      ]
      
    return CANParser(DBC[CP.carFingerprint]["pt"], signals, checks, 0)

  @staticmethod
  def get_cam_can_parser(CP):
    signals = []
    checks = []

    if CP.carFingerprint in GEN1:
      signals += [
        # sig_name, sig_address
        ("LKAS_REQUEST", "CAM_LKAS"),
        ("CTR", "CAM_LKAS"),
        ("ERR_BIT_1", "CAM_LKAS"),
        ("LINE_NOT_VISIBLE", "CAM_LKAS"),
        ("BIT_1", "CAM_LKAS"),
        ("ERR_BIT_2", "CAM_LKAS"),
        ("STEERING_ANGLE", "CAM_LKAS"),
        ("ANGLE_ENABLED", "CAM_LKAS"),
        ("CHKSUM", "CAM_LKAS"),

        ("LINE_VISIBLE", "CAM_LANEINFO"),
        ("LINE_NOT_VISIBLE", "CAM_LANEINFO"),
        ("LANE_LINES", "CAM_LANEINFO"),
        ("BIT1", "CAM_LANEINFO"),
        ("BIT2", "CAM_LANEINFO"),
        ("BIT3", "CAM_LANEINFO"),
        ("NO_ERR_BIT", "CAM_LANEINFO"),
        ("S1", "CAM_LANEINFO"),
        ("S1_HBEAM", "CAM_LANEINFO"),
      ]

      checks += [
        # sig_address, frequency
        ("CAM_LANEINFO", 2),
        ("CAM_LKAS", 16),
      ]

    return CANParser(DBC[CP.carFingerprint]["pt"], signals, checks, 2)


  @staticmethod
  def get_body_can_parser(CP):
    # this function generates lists for signal, messages and initial values
    signals = []
    checks = []
    # get real driver torque if we are using a torque interceptor
    if CP.enableTorqueInterceptor:
      signals += [
        ("TI_TORQUE_SENSOR", "TI_FEEDBACK", 0),
        ("CHKSUM", "TI_FEEDBACK", 0),
        ("VERSION_NUMBER", "TI_FEEDBACK", 0),
        ("STATE", "TI_FEEDBACK", 0),
        ("VIOL", "TI_FEEDBACK", 0),
        ("ERROR", "TI_FEEDBACK", 0),
        ("RAMP_DOWN", "TI_FEEDBACK", 0),
      ]

      checks += [
        ("TI_FEEDBACK", 50),
      ]

    return CANParser(DBC[CP.carFingerprint]["pt"], signals, checks, 1) # changed to back to 0 because my OBD2 port works
