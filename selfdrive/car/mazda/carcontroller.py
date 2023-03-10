from cereal import car
from opendbc.can.packer import CANPacker
from selfdrive.car import apply_std_steer_torque_limits
from selfdrive.car.mazda import mazdacan
from selfdrive.car.mazda.values import CarControllerParams, TiSteerLimits, Buttons
from common.params import Params

VisualAlert = car.CarControl.HUDControl.VisualAlert


class CarController:
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.apply_steer_last = 0
    self.packer = CANPacker(dbc_name)
    self.brake_counter = 0
    self.frame = 0
    # dp - ti
    self.dp_mazda_ti = Params().get_bool('dp_mazda_ti')
    self.ti_apply_steer_last = 0

  def update(self, CC, CS, now_nanos):
    can_sends = []

    apply_steer = 0
    ti_apply_steer = 0

    if CC.latActive:
      # ti
      if self.dp_mazda_ti and CS.CP.enableTorqueInterceptor:
        if CS.ti_lkas_allowed:
          ti_new_steer = int(round(CC.actuators.steer * TiSteerLimits.TI_STEER_MAX))
          ti_apply_steer = (ti_new_steer, self.ti_apply_steer_last,
                                                      CS.out.steeringTorque, CarControllerParams)

      new_steer = int(round(CC.actuators.steer * CarControllerParams.STEER_MAX))
      apply_steer = apply_std_steer_torque_limits(new_steer, self.apply_steer_last,
                                                  CS.out.steeringTorque, CarControllerParams)

    if CC.cruiseControl.cancel:
      # If brake is pressed, let us wait >70ms before trying to disable crz to avoid
      # a race condition with the stock system, where the second cancel from openpilot
      # will disable the crz 'main on'. crz ctrl msg runs at 50hz. 70ms allows us to
      # read 3 messages and most likely sync state before we attempt cancel.
      self.brake_counter = self.brake_counter + 1
      if self.frame % 10 == 0 and not (CS.out.brakePressed and self.brake_counter < 7):
        # Cancel Stock ACC if it's enabled while OP is disengaged
        # Send at a rate of 10hz until we sync with stock ACC state
        can_sends.append(mazdacan.create_button_cmd(self.packer, self.CP.carFingerprint, CS.crz_btns_counter, Buttons.CANCEL))
    else:
      self.brake_counter = 0
      if CC.cruiseControl.resume and self.frame % 5 == 0:
        # Mazda Stop and Go requires a RES button (or gas) press if the car stops more than 3 seconds
        # Send Resume button when planner wants car to move
        can_sends.append(mazdacan.create_button_cmd(self.packer, self.CP.carFingerprint, CS.crz_btns_counter, Buttons.RESUME))

    self.apply_steer_last = apply_steer
    self.ti_apply_steer_last = ti_apply_steer

    # send HUD alerts
    if self.frame % 50 == 0:
      ldw = CC.hudControl.visualAlert == VisualAlert.ldw
      if not self.dp_mazda_ti:
        steer_required = CC.hudControl.visualAlert == VisualAlert.steerRequired
        # TODO: find a way to silence audible warnings so we can add more hud alerts
        steer_required = steer_required and CS.lkas_allowed_speed
      else:
        steer_required = CS.out.steerWarning
      can_sends.append(mazdacan.create_alert_command(self.packer, CS.cam_laneinfo, ldw, steer_required))

    # ti
    #The ti cannot be detected unless OP sends a can message to it becasue the ti only transmits when it
    #sees the signature key in the designated address range.
    can_sends.append(mazdacan.create_ti_steering_control(self.packer, CS.CP.carFingerprint, self.frame, ti_apply_steer))
    # Always send steering command to stock system, even with TI enabled.
    can_sends.append(mazdacan.create_steering_control(self.packer, self.CP.carFingerprint,
                                                      self.frame, apply_steer, CS.cam_lkas))

    new_actuators = CC.actuators.copy()
    new_actuators.steer = apply_steer / CarControllerParams.STEER_MAX
    new_actuators.steerOutputCan = apply_steer

    self.frame += 1
    return new_actuators, can_sends
