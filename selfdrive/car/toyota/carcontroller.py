from cereal import car
from common.numpy_fast import clip, interp
from selfdrive.car import apply_meas_steer_torque_limits, apply_std_steer_angle_limits, \
                          create_gas_interceptor_command, make_can_msg
from selfdrive.car.toyota.toyotacan import create_steer_command, create_ui_command, \
                                           create_accel_command, create_acc_cancel_command, \
                                           create_fcw_command, create_lta_steer_command
from selfdrive.car.toyota.values import CAR, STATIC_DSU_MSGS, NO_STOP_TIMER_CAR, TSS2_CAR, \
                                        MIN_ACC_SPEED, PEDAL_TRANSITION, CarControllerParams, \
                                        UNSUPPORTED_DSU_CAR
from opendbc.can.packer import CANPacker
from common.conversions import Conversions as CV
from common.params import Params

SteerControlType = car.CarParams.SteerControlType
VisualAlert = car.CarControl.HUDControl.VisualAlert

# LKA limits
# EPS faults if you apply torque while the steering rate is above 100 deg/s for too long
MAX_STEER_RATE = 100  # deg/s
MAX_STEER_RATE_FRAMES = 18  # tx control frames needed before torque can be cut

# EPS allows user torque above threshold for 50 frames before permanently faulting
MAX_USER_TORQUE = 500

# LTA limits
# EPS ignores commands above this angle and causes PCS to fault
MAX_STEER_ANGLE = 94.9461  # deg

GearShifter = car.CarState.GearShifter
UNLOCK_CMD = b'\x40\x05\x30\x11\x00\x40\x00\x00'
LOCK_CMD = b'\x40\x05\x30\x11\x00\x80\x00\x00'
LOCK_AT_SPEED = 10 * CV.KPH_TO_MS

# Blindspot codes
LEFT_BLINDSPOT = b'\x41'
RIGHT_BLINDSPOT = b'\x42'

def set_blindspot_debug_mode(lr,enable):
  if enable:
    m = lr + b'\x02\x10\x60\x00\x00\x00\x00'
  else:
    m = lr + b'\x02\x10\x01\x00\x00\x00\x00'
  return make_can_msg(0x750, m, 0)


def poll_blindspot_status(lr):
  m = lr + b'\x02\x21\x69\x00\x00\x00\x00'
  return make_can_msg(0x750, m, 0)

class CarController:
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.params = CarControllerParams(self.CP)
    self.frame = 0
    self.last_steer = 0
    self.last_angle = 0
    self.alert_active = False
    self.last_standstill = False
    self.standstill_req = False
    self.steer_rate_counter = 0

    self.steer_rate_counter = 0

    self.packer = CANPacker(dbc_name)
    self.gas = 0
    self.accel = 0

    # dp
    self.dp_toyota_sng = False

    self.dp_toyota_auto_lock = False
    self.dp_toyota_auto_unlock = False
    self.last_gear = GearShifter.park
    self.lock_once = False
    self.lat_controller_type = None
    self.lat_controller_type_prev = None
    self.blindspot_debug_enabled_left = False
    self.blindspot_debug_enabled_right = False
    self.blindspot_frame = 0
    if self.CP.carFingerprint in TSS2_CAR: # tss2 can do higher hz then tss1 and can be on at all speed/standstill
        self.blindspot_rate = 2
        self.blindspot_always_on = True
    else:
        self.blindspot_rate = 20
        self.blindspot_always_on = False

  def update(self, CC, CS, now_nanos, dragonconf):
    if dragonconf is not None:
      self.dp_toyota_sng = dragonconf.dpToyotaSng
      self.dp_toyota_auto_lock = dragonconf.dpToyotaAutoLock
      self.dp_toyota_auto_unlock = dragonconf.dpToyotaAutoUnlock
      self.dp_toyota_debug_bsm = dragonconf.dpToyotaDebugBsm
    self.lat_controller_type = CC.latController
    if self.lat_controller_type != self.lat_controller_type_prev:
      self.torque_rate_limits.update(CC.latController)
    self.lat_controller_type_prev = self.lat_controller_type

    self.dp_toyota_change5speed = Params().get_bool("dp_toyota_change5speed")
    actuators = CC.actuators
    hud_control = CC.hudControl
    pcm_cancel_cmd = CC.cruiseControl.cancel
    lat_active = CC.latActive and abs(CS.out.steeringTorque) < MAX_USER_TORQUE

    # *** control msgs ***
    can_sends = []

    # *** steer torque ***
    new_steer = int(round(actuators.steer * self.params.STEER_MAX))
    apply_steer = apply_meas_steer_torque_limits(new_steer, self.last_steer, CS.out.steeringTorqueEps, self.params)

    # Count up to MAX_STEER_RATE_FRAMES, at which point we need to cut torque to avoid a steering fault
    if lat_active and abs(CS.out.steeringRateDeg) >= MAX_STEER_RATE:
      self.steer_rate_counter += 1
    else:
      self.steer_rate_counter = 0

    apply_steer_req = 1
    if not lat_active:
      apply_steer = 0
      apply_steer_req = 0
    elif self.steer_rate_counter > MAX_STEER_RATE_FRAMES:
      apply_steer_req = 0
      self.steer_rate_counter = 0

    # *** steer angle ***
    if self.CP.steerControlType == SteerControlType.angle:
      # If using LTA control, disable LKA and set steering angle command
      apply_steer = 0
      apply_steer_req = 0
      if self.frame % 2 == 0:
        # EPS uses the torque sensor angle to control with, offset to compensate
        apply_angle = actuators.steeringAngleDeg + CS.out.steeringAngleOffsetDeg

        # Angular rate limit based on speed
        apply_angle = apply_std_steer_angle_limits(apply_angle, self.last_angle, CS.out.vEgo, self.params)

        if not CC.latActive:
          apply_angle = CS.out.steeringAngleDeg + CS.out.steeringAngleOffsetDeg

        self.last_angle = clip(apply_angle, -MAX_STEER_ANGLE, MAX_STEER_ANGLE)

    self.last_steer = apply_steer

    # toyota can trace shows this message at 42Hz, with counter adding alternatively 1 and 2;
    # sending it at 100Hz seem to allow a higher rate limit, as the rate limit seems imposed
    # on consecutive messages
    can_sends.append(create_steer_command(self.packer, apply_steer, apply_steer_req))
    if self.frame % 2 == 0 and self.CP.carFingerprint in TSS2_CAR:
      lta_active = CC.latActive and self.CP.steerControlType == SteerControlType.angle
      can_sends.append(create_lta_steer_command(self.packer, self.last_angle, lta_active, self.frame // 2))

    # *** gas and brake ***
    if self.CP.enableGasInterceptor and CC.longActive:
      MAX_INTERCEPTOR_GAS = 0.5
      # RAV4 has very sensitive gas pedal
      if self.CP.carFingerprint in (CAR.RAV4, CAR.RAV4H, CAR.HIGHLANDER, CAR.HIGHLANDERH):
        PEDAL_SCALE = interp(CS.out.vEgo, [0.0, MIN_ACC_SPEED, MIN_ACC_SPEED + PEDAL_TRANSITION], [0.15, 0.3, 0.0])
      elif self.CP.carFingerprint in (CAR.COROLLA,):
        PEDAL_SCALE = interp(CS.out.vEgo, [0.0, MIN_ACC_SPEED, MIN_ACC_SPEED + PEDAL_TRANSITION], [0.3, 0.4, 0.0])
      else:
        PEDAL_SCALE = interp(CS.out.vEgo, [0.0, MIN_ACC_SPEED, MIN_ACC_SPEED + PEDAL_TRANSITION], [0.4, 0.5, 0.0])
      # offset for creep and windbrake
      pedal_offset = interp(CS.out.vEgo, [0.0, 2.3, MIN_ACC_SPEED + PEDAL_TRANSITION], [-.4, 0.0, 0.2])
      pedal_command = PEDAL_SCALE * (actuators.accel + pedal_offset)
      interceptor_gas_cmd = clip(pedal_command, 0., MAX_INTERCEPTOR_GAS)
    else:
      interceptor_gas_cmd = 0.
    pcm_accel_cmd = clip(actuators.accel, self.params.ACCEL_MIN, self.params.ACCEL_MAX)

    # TODO: probably can delete this. CS.pcm_acc_status uses a different signal
    # than CS.cruiseState.enabled. confirm they're not meaningfully different
    if not CC.enabled and CS.pcm_acc_status:
      pcm_cancel_cmd = 1

    # on entering standstill, send standstill request
    if CS.out.standstill and not self.last_standstill and (self.CP.carFingerprint not in NO_STOP_TIMER_CAR or self.CP.enableGasInterceptor):
      self.standstill_req = True
    if CS.pcm_acc_status != 8:
      # pcm entered standstill or it's disabled
      self.standstill_req = False
    self.standstill_req = False if self.dp_toyota_sng else self.standstill_req

    self.last_standstill = CS.out.standstill

    # dp - door auto lock / unlock logic
    # thanks to AlexandreSato & cydia2020
    # https://github.com/AlexandreSato/animalpilot/blob/personal/doors.py
    if self.dp_toyota_auto_lock or self.dp_toyota_auto_unlock:
      gear = CS.out.gearShifter
      if self.last_gear != gear and gear == GearShifter.park:
        if self.dp_toyota_auto_unlock:
          can_sends.append(make_can_msg(0x750, UNLOCK_CMD, 0))
        if self.dp_toyota_auto_lock:
          self.lock_once = False
      elif self.dp_toyota_auto_lock and not CS.out.doorOpen and gear == GearShifter.drive and not self.lock_once and CS.out.vEgo >= LOCK_AT_SPEED:
        can_sends.append(make_can_msg(0x750, LOCK_CMD, 0))
        self.lock_once = True
      self.last_gear = gear

    # Enable blindspot debug mode once (@arne182)
    # let's keep all the commented out code for easy debug purpose for future.
    if self.dp_toyota_debug_bsm:
      if self.frame > 2000:
        #left bsm
        if not self.blindspot_debug_enabled_left:
            if (self.blindspot_always_on or (CS.out.leftBlinker and CS.out.vEgo > 6)): # eagle eye camera will stop working if right bsm is switched on under 6m/s
                can_sends.append(set_blindspot_debug_mode(LEFT_BLINDSPOT, True))
                self.blindspot_debug_enabled_left = True
                # print("bsm debug left, on")
        else:
            if not self.blindspot_always_on and not CS.out.leftBlinker and self.frame - self.blindspot_frame > 500:
                can_sends.append(set_blindspot_debug_mode(LEFT_BLINDSPOT, False))
                self.blindspot_debug_enabled_left = False
                # print("bsm debug left, off")
            if self.frame % self.blindspot_rate == 0:
                can_sends.append(poll_blindspot_status(LEFT_BLINDSPOT))
                if CS.out.leftBlinker:
                    self.blindspot_frame = self.frame
                    # print(self.blindspot_frame)
                # print("bsm poll left")
        #right bsm
        if not self.blindspot_debug_enabled_right:
            if (self.blindspot_always_on or (CS.out.rightBlinker and CS.out.vEgo > 6)): # eagle eye camera will stop working if right bsm is switched on under 6m/s
                can_sends.append(set_blindspot_debug_mode(RIGHT_BLINDSPOT, True))
                self.blindspot_debug_enabled_right = True
                # print("bsm debug right, on")
        else:
            if not self.blindspot_always_on and not CS.out.rightBlinker and self.frame - self.blindspot_frame > 500:
                can_sends.append(set_blindspot_debug_mode(RIGHT_BLINDSPOT, False))
                self.blindspot_debug_enabled_right = False
                # print("bsm debug right, off")
            if self.frame % self.blindspot_rate == self.blindspot_rate/2:
                can_sends.append(poll_blindspot_status(RIGHT_BLINDSPOT))
                if CS.out.rightBlinker:
                    self.blindspot_frame = self.frame
                    # print(self.blindspot_frame)
                # print("bsm poll right")

    # we can spam can to cancel the system even if we are using lat only control
    if (self.frame % 3 == 0 and self.CP.openpilotLongitudinalControl) or pcm_cancel_cmd:
      lead = hud_control.leadVisible or CS.out.vEgo < 12.  # at low speed we always assume the lead is present so ACC can be engaged

      # Lexus IS uses a different cancellation message
      if pcm_cancel_cmd and self.CP.carFingerprint in UNSUPPORTED_DSU_CAR:
        can_sends.append(create_acc_cancel_command(self.packer))
      elif self.CP.openpilotLongitudinalControl:
        can_sends.append(create_accel_command(self.packer, pcm_accel_cmd, pcm_cancel_cmd, self.standstill_req, lead, CS.acc_type, CS.distance, self.dp_toyota_change5speed))
        self.accel = pcm_accel_cmd
      else:
        can_sends.append(create_accel_command(self.packer, 0, pcm_cancel_cmd, False, lead, CS.acc_type, CS.distance, self.dp_toyota_change5speed))

    if self.frame % 2 == 0 and self.CP.enableGasInterceptor and self.CP.openpilotLongitudinalControl:
      # send exactly zero if gas cmd is zero. Interceptor will send the max between read value and gas cmd.
      # This prevents unexpected pedal range rescaling
      can_sends.append(create_gas_interceptor_command(self.packer, interceptor_gas_cmd, self.frame // 2))
      self.gas = interceptor_gas_cmd

    # *** hud ui ***
    if self.CP.carFingerprint != CAR.PRIUS_V:
      # ui mesg is at 1Hz but we send asap if:
      # - there is something to display
      # - there is something to stop displaying
      fcw_alert = hud_control.visualAlert == VisualAlert.fcw
      steer_alert = hud_control.visualAlert in (VisualAlert.steerRequired, VisualAlert.ldw)

      send_ui = False
      if ((fcw_alert or steer_alert) and not self.alert_active) or \
         (not (fcw_alert or steer_alert) and self.alert_active):
        send_ui = True
        self.alert_active = not self.alert_active
      elif pcm_cancel_cmd:
        # forcing the pcm to disengage causes a bad fault sound so play a good sound instead
        send_ui = True

      if self.frame % 20 == 0 or send_ui:
        can_sends.append(create_ui_command(self.packer, steer_alert, pcm_cancel_cmd, hud_control.leftLaneVisible,
                                           hud_control.rightLaneVisible, hud_control.leftLaneDepart,
                                           hud_control.rightLaneDepart, CC.latActive, CS.lkas_hud))

      if (self.frame % 100 == 0 or send_ui) and self.CP.enableDsu:
        can_sends.append(create_fcw_command(self.packer, fcw_alert))

    # *** static msgs ***
    for addr, cars, bus, fr_step, vl in STATIC_DSU_MSGS:
      if self.frame % fr_step == 0 and self.CP.enableDsu and self.CP.carFingerprint in cars:
        can_sends.append(make_can_msg(addr, vl, bus))

    new_actuators = actuators.copy()
    new_actuators.steer = apply_steer / self.params.STEER_MAX
    new_actuators.steerOutputCan = apply_steer
    new_actuators.steeringAngleDeg = self.last_angle
    new_actuators.accel = self.accel
    new_actuators.gas = self.gas

    self.frame += 1
    return new_actuators, can_sends
