#!/usr/bin/env python3
from cereal import car
from common.conversions import Conversions as CV
from selfdrive.car.mazda.values import CAR, LKAS_LIMITS
from selfdrive.car import STD_CARGO_KG, scale_tire_stiffness, get_safety_config
from selfdrive.car.interfaces import CarInterfaceBase
from selfdrive import global_ti as TI
from common.params import Params

ButtonType = car.CarState.ButtonEvent.Type
EventName = car.CarEvent.EventName

class CarInterface(CarInterfaceBase):
	def __init__(self, CP, CarController, CarState):
    super().__init__(CP, CarController, CarState)
    self.dp_mazda_ti = Params().get_bool('dp_mazda_ti')

  @staticmethod
  def _get_params(ret, candidate, fingerprint, car_fw, experimental_long):
    ret.carName = "mazda"

    ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.mazda)]
    ret.radarUnavailable = True

    ret.dashcamOnly = candidate not in (CAR.CX5_2022, CAR.CX9_2021) and not Params().get_bool('dp_mazda_dashcam_bypass')

    ret.steerActuatorDelay = 0.1
    ret.steerLimitTimer = 0.8
    tire_stiffness_factor = 0.70   # not optimized yet

    CarInterfaceBase.configure_torque_tune(candidate, ret.lateralTuning)

    if candidate in (CAR.CX5, CAR.CX5_2022):
      ret.mass = 3655 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.7
      ret.steerRatio = 15.5
    elif candidate in (CAR.CX9, CAR.CX9_2021):
      ret.mass = 4217 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 3.1
      ret.steerRatio = 17.6
    elif candidate == CAR.MAZDA3:
      ret.mass = 2875 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.7
      ret.steerRatio = 14.0
    elif candidate == CAR.MAZDA6:
      ret.mass = 3443 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.83
      ret.steerRatio = 15.5

    if self.CP.enableTorqueInterceptor and self.dp_mazda_ti:
      if candidate in (CAR.CX5, CAR.CX5_2022):
        ret.lateralTuning.pid.kiBP = [5.0, 25.0]
        ret.lateralTuning.pid.kpBP = [5.0, 25.0]
        ret.lateralTuning.pid.kpV = [0.25,0.28]
        ret.lateralTuning.pid.kiV = [0.01,0.025]
        ret.lateralTuning.pid.kf = 0.00008

        ret.lateralTuning.init('indi')
        ret.lateralTuning.indi.innerLoopGainBP = [5.0, 35]
        ret.lateralTuning.indi.innerLoopGainV = [4.5, 6.0]
        ret.lateralTuning.indi.outerLoopGainBP = [5, 35]
        ret.lateralTuning.indi.outerLoopGainV = [3.0, 6]
        ret.lateralTuning.indi.timeConstantBP = [2, 35]
        ret.lateralTuning.indi.timeConstantV = [0.2, 1.5]
        ret.lateralTuning.indi.actuatorEffectivenessBP = [0, 25]
        ret.lateralTuning.indi.actuatorEffectivenessV = [2.0, 1]
      elif candidate in [CAR.CX9, CAR.CX9_2021]:
        ret.lateralTuning.pid.kiBP = [8.0, 30.0]
        ret.lateralTuning.pid.kpBP = [8.0, 30.0]
        ret.lateralTuning.pid.kpV = [0.10,0.22]
        ret.lateralTuning.pid.kiV = [0.01,0.019]
        ret.lateralTuning.pid.kf = 0.00006
      elif candidate == CAR.MAZDA3:
        ret.lateralTuning.pid.kiBP = [5.0, 25.0]
        ret.lateralTuning.pid.kpBP = [5.0, 25.0]
        ret.lateralTuning.pid.kpV = [0.25,0.28]
        ret.lateralTuning.pid.kiV = [0.01,0.025]
        ret.lateralTuning.pid.kf = 0.00008

        ret.lateralTuning.init('indi')
        ret.lateralTuning.indi.innerLoopGainBP = [5.0, 35]
        ret.lateralTuning.indi.innerLoopGainV = [4.5, 6.0]
        ret.lateralTuning.indi.outerLoopGainBP = [5, 35]
        ret.lateralTuning.indi.outerLoopGainV = [3.0, 6]
        ret.lateralTuning.indi.timeConstantBP = [2, 35]
        ret.lateralTuning.indi.timeConstantV = [0.2, 1.5]
        ret.lateralTuning.indi.actuatorEffectivenessBP = [0, 25]
        ret.lateralTuning.indi.actuatorEffectivenessV = [2.0, 1]
      elif candidate == CAR.MAZDA6:
        ret.lateralTuning.pid.kiBP = [8.0, 30.0]
        ret.lateralTuning.pid.kpBP = [8.0, 30.0]
        ret.lateralTuning.pid.kpV = [0.10,0.22]
        ret.lateralTuning.pid.kiV = [0.01,0.019]
        ret.lateralTuning.pid.kf = 0.00006

    if candidate not in (CAR.CX5_2022, ):
      ret.minSteerSpeed = LKAS_LIMITS.DISABLE_SPEED * CV.KPH_TO_MSS

    CarInterfaceBase.configure_dp_tune(candidate, ret.lateralTuning)

    ret.centerToFront = ret.wheelbase * 0.41

    # TODO: start from empirically derived lateral slip stiffness for the civic and scale by
    # mass and CG position, so all cars will have approximately similar dyn behaviors
    ret.tireStiffnessFront, ret.tireStiffnessRear = scale_tire_stiffness(ret.mass, ret.wheelbase, ret.centerToFront,
                                                                         tire_stiffness_factor=tire_stiffness_factor)

    return ret

  # returns a car.CarState
  def _update(self, c):
    if self.CP.enableTorqueInterceptor and not TI.enabled:
      TI.enabled = True
      self.cp_body = self.CS.get_body_can_parser(self.CP)
      self.can_parsers = [self.cp, self.cp_cam, self.cp_adas, self.cp_body, self.cp_loopback]

    ret = self.CS.update(self.cp, self.cp_cam, self.cp_body)
	ret.cruiseState.enabled, ret.cruiseState.available = self.dp_atl_mode(ret)

    # events
    events = self.create_common_events(ret)
    events = self.dp_atl_warning(ret, events)

    if self.CS.lkas_disabled:
      events.add(EventName.lkasDisabled)
    elif self.dragonconf.dpMazdaSteerAlert and self.CS.low_speed_alert:
      events.add(EventName.belowSteerSpeed)

    if self.dp_mazda_ti and not self.CS.acc_active_last and not self.CS.ti_lkas_allowed:
      events.add(EventName.steerTempUnavailable)

    ret.events = events.to_msg()

    return ret

  def apply(self, c, now_nanos):
    return self.CC.update(c, self.CS, now_nanos)
