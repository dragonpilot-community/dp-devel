#!/usr/bin/env python3
from cereal import car
from common.conversions import Conversions as CV
from selfdrive.car.mazda.values import CAR, LKAS_LIMITS, GEN2, GEN1
from selfdrive.car import STD_CARGO_KG, scale_tire_stiffness, get_safety_config
from selfdrive.controls.lib.drive_helpers import get_friction
from selfdrive.car.interfaces import CarInterfaceBase, FRICTION_THRESHOLD
from common.params import Params
from typing import Callable

ButtonType = car.CarState.ButtonEvent.Type
EventName = car.CarEvent.EventName
TorqueFromLateralAccelCallbackType = Callable[[float, car.CarParams.LateralTorqueTuning, float, float, float, float, bool], float]

class CarInterface(CarInterfaceBase):
  
  @staticmethod
  def torque_from_lateral_accel_ti(lateral_accel_value: float, torque_params: car.CarParams.LateralTorqueTuning,
                                           lateral_accel_error: float, lateral_accel_deadzone: float,
                                           steering_angle: float, vego: float, friction_compensation: bool) -> float:
    steering_angle = abs(steering_angle)
    lat_factor = torque_params.latAccelFactor * ((steering_angle * torque_params.latAngleFactor) + 1)
    
    friction = get_friction(lateral_accel_error, lateral_accel_deadzone, FRICTION_THRESHOLD, torque_params, friction_compensation)
    return (lateral_accel_value / lat_factor) + friction

  def torque_from_lateral_accel(self):
    if self.CP.carFingerprint in GEN2:
      return self.torque_from_lateral_accel_ti # different return type
    else:
      return self.torque_from_lateral_accel_linear

  @staticmethod
  def _get_params(ret, candidate, fingerprint, car_fw, experimental_long, docs):
    ret.carName = "mazda"
    if candidate in GEN1:
      ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.mazda)]
      
    if candidate in GEN2:
      ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.mazda2019)]
      ret.openpilotLongitudinalControl = True
      ret.stopAccel = -.5
      ret.vEgoStarting = .2
      ret.longitudinalTuning.kpBP = [0., 5., 35.]
      ret.longitudinalTuning.kpV = [0.0, 0.0, 0.0]
      ret.longitudinalTuning.kiBP = [0., 35.]
      ret.longitudinalTuning.kiV = [0.1, 0.1]
      ret.startingState = True
      
    ret.radarUnavailable = True

    ret.dashcamOnly = candidate not in (CAR.CX5_2022, CAR.CX9_2021) and not Params().get_bool('dp_mazda_dashcam_bypass')

    ret.steerActuatorDelay = 0.1
    ret.steerLimitTimer = 0.8
    tire_stiffness_factor = 0.70   # not optimized yet

    CarInterfaceBase.configure_torque_tune(candidate, ret.lateralTuning)
    CarInterfaceBase.configure_ti_tune(candidate, ret.lateralTuning)

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
    elif candidate in CAR.MAZDA3_2019:
      ret.mass = 3000 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.725
      ret.steerRatio = 17.0
    elif candidate in (CAR.CX_30, CAR.CX_50):
      ret.mass = 3375 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.7
      ret.steerRatio = 15.5
    elif candidate in (CAR.CX_60, CAR.CX_80, CAR.CX_70, CAR.CX_90):
      ret.mass = 4217 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 3.1
      ret.steerRatio = 17.6

    if candidate not in (CAR.CX5_2022, CAR.MAZDA3_2019, CAR.CX_30, CAR.CX_50, CAR.CX_60, CAR.CX_70, CAR.CX_80, CAR.CX_90):
      ret.minSteerSpeed = LKAS_LIMITS.DISABLE_SPEED * CV.KPH_TO_MS

    CarInterfaceBase.configure_dp_tune(candidate, ret.lateralTuning)

    ret.centerToFront = ret.wheelbase * 0.41

    # TODO: start from empirically derived lateral slip stiffness for the civic and scale by
    # mass and CG position, so all cars will have approximately similar dyn behaviors
    ret.tireStiffnessFront, ret.tireStiffnessRear = scale_tire_stiffness(ret.mass, ret.wheelbase, ret.centerToFront,
                                                                         tire_stiffness_factor=tire_stiffness_factor)

    return ret

  # returns a car.CarState
  def _update(self, c):
    ret = self.CS.update(self.cp, self.cp_cam, self.cp_body)
    ret.cruiseState.enabled, ret.cruiseState.available = self.dp_atl_mode(ret)

    # events
    events = self.create_common_events(ret)
    events = self.dp_atl_warning(ret, events)

    if self.CS.lkas_disabled:
      events.add(EventName.lkasDisabled)
    elif self.dragonconf.dpMazdaSteerAlert and self.CS.low_speed_alert:
      events.add(EventName.belowSteerSpeed)

    ret.events = events.to_msg()

    return ret

  def apply(self, c, now_nanos):
    return self.CC.update(c, self.CS, now_nanos)
