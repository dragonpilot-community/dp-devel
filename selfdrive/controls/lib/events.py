# This Python file uses the following encoding: utf-8
# -*- coding: utf-8 -*-
from common.i18n import events
_ = events()

import math
import os
from enum import IntEnum
from typing import Dict, Union, Callable, List, Optional

from cereal import log, car
import cereal.messaging as messaging
from common.conversions import Conversions as CV
from common.realtime import DT_CTRL
from selfdrive.locationd.calibrationd import MIN_SPEED_FILTER
from system.version import get_short_branch

AlertSize = log.ControlsState.AlertSize
AlertStatus = log.ControlsState.AlertStatus
VisualAlert = car.CarControl.HUDControl.VisualAlert
AudibleAlert = car.CarControl.HUDControl.AudibleAlert
EventName = car.CarEvent.EventName


# Alert priorities
class Priority(IntEnum):
  LOWEST = 0
  LOWER = 1
  LOW = 2
  MID = 3
  HIGH = 4
  HIGHEST = 5


# Event types
class ET:
  ENABLE = 'enable'
  PRE_ENABLE = 'preEnable'
  OVERRIDE_LATERAL = 'overrideLateral'
  OVERRIDE_LONGITUDINAL = 'overrideLongitudinal'
  NO_ENTRY = 'noEntry'
  WARNING = 'warning'
  USER_DISABLE = 'userDisable'
  SOFT_DISABLE = 'softDisable'
  IMMEDIATE_DISABLE = 'immediateDisable'
  PERMANENT = 'permanent'


# get event name from enum
EVENT_NAME = {v: k for k, v in EventName.schema.enumerants.items()}


class Events:
  def __init__(self):
    self.events: List[int] = []
    self.static_events: List[int] = []
    self.events_prev = dict.fromkeys(EVENTS.keys(), 0)

  @property
  def names(self) -> List[int]:
    return self.events

  def __len__(self) -> int:
    return len(self.events)

  def add(self, event_name: int, static: bool=False) -> None:
    if static:
      self.static_events.append(event_name)
    self.events.append(event_name)

  def clear(self) -> None:
    self.events_prev = {k: (v + 1 if k in self.events else 0) for k, v in self.events_prev.items()}
    self.events = self.static_events.copy()

  def any(self, event_type: str) -> bool:
    return any(event_type in EVENTS.get(e, {}) for e in self.events)

  def create_alerts(self, event_types: List[str], callback_args=None):
    if callback_args is None:
      callback_args = []

    ret = []
    for e in self.events:
      types = EVENTS[e].keys()
      for et in event_types:
        if et in types:
          alert = EVENTS[e][et]
          if not isinstance(alert, Alert):
            alert = alert(*callback_args)

          if DT_CTRL * (self.events_prev[e] + 1) >= alert.creation_delay:
            alert.alert_type = f"{EVENT_NAME[e]}/{et}"
            alert.event_type = et
            ret.append(alert)
    return ret

  def add_from_msg(self, events):
    for e in events:
      self.events.append(e.name.raw)

  def to_msg(self):
    ret = []
    for event_name in self.events:
      event = car.CarEvent.new_message()
      event.name = event_name
      for event_type in EVENTS.get(event_name, {}):
        setattr(event, event_type, True)
      ret.append(event)
    return ret


class Alert:
  def __init__(self,
               alert_text_1: str,
               alert_text_2: str,
               alert_status: log.ControlsState.AlertStatus,
               alert_size: log.ControlsState.AlertSize,
               priority: Priority,
               visual_alert: car.CarControl.HUDControl.VisualAlert,
               audible_alert: car.CarControl.HUDControl.AudibleAlert,
               duration: float,
               alert_rate: float = 0.,
               creation_delay: float = 0.):

    self.alert_text_1 = alert_text_1
    self.alert_text_2 = alert_text_2
    self.alert_status = alert_status
    self.alert_size = alert_size
    self.priority = priority
    self.visual_alert = visual_alert
    self.audible_alert = audible_alert

    self.duration = int(duration / DT_CTRL)

    self.alert_rate = alert_rate
    self.creation_delay = creation_delay

    self.alert_type = ""
    self.event_type: Optional[str] = None

  def __str__(self) -> str:
    return f"{self.alert_text_1}/{self.alert_text_2} {self.priority} {self.visual_alert} {self.audible_alert}"

  def __gt__(self, alert2) -> bool:
    if not isinstance(alert2, Alert):
      return False
    return self.priority > alert2.priority


class NoEntryAlert(Alert):
  def __init__(self, alert_text_2: str,
               alert_text_1: str = _("openpilot Unavailable"),
               visual_alert: car.CarControl.HUDControl.VisualAlert=VisualAlert.none):
    super().__init__(alert_text_1, alert_text_2, AlertStatus.normal,
                     AlertSize.mid, Priority.LOW, visual_alert,
                     AudibleAlert.refuse, 3.)


class SoftDisableAlert(Alert):
  def __init__(self, alert_text_2: str):
    super().__init__(_("TAKE CONTROL IMMEDIATELY"), alert_text_2,
                     AlertStatus.userPrompt, AlertSize.full,
                     Priority.MID, VisualAlert.steerRequired,
                     AudibleAlert.warningSoft, 2.),


# less harsh version of SoftDisable, where the condition is user-triggered
class UserSoftDisableAlert(SoftDisableAlert):
  def __init__(self, alert_text_2: str):
    super().__init__(alert_text_2),
    self.alert_text_1 = _("openpilot will disengage")


class ImmediateDisableAlert(Alert):
  def __init__(self, alert_text_2: str):
    super().__init__(_("TAKE CONTROL IMMEDIATELY"), alert_text_2,
                     AlertStatus.critical, AlertSize.full,
                     Priority.HIGHEST, VisualAlert.steerRequired,
                     AudibleAlert.warningImmediate, 4.),


class EngagementAlert(Alert):
  def __init__(self, audible_alert: car.CarControl.HUDControl.AudibleAlert):
    super().__init__("", "",
                     AlertStatus.normal, AlertSize.none,
                     Priority.MID, VisualAlert.none,
                     audible_alert, .2),


class NormalPermanentAlert(Alert):
  def __init__(self, alert_text_1: str, alert_text_2: str = "", duration: float = 0.2, priority: Priority = Priority.LOWER, creation_delay: float = 0.):
    super().__init__(alert_text_1, alert_text_2,
                     AlertStatus.normal, AlertSize.mid if len(alert_text_2) else AlertSize.small,
                     priority, VisualAlert.none, AudibleAlert.none, duration, creation_delay=creation_delay),


class StartupAlert(Alert):
  def __init__(self, alert_text_1: str, alert_text_2: str = _("Always keep hands on wheel and eyes on road"), alert_status=AlertStatus.normal):
    super().__init__(alert_text_1, alert_text_2,
                     alert_status, AlertSize.mid,
                     Priority.LOWER, VisualAlert.none, AudibleAlert.none, 5.),


# ********** helper functions **********
def get_display_speed(speed_ms: float, metric: bool) -> str:
  speed = int(round(speed_ms * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH)))
  unit = 'km/h' if metric else 'mph'
  return f"{speed} {unit}"


# ********** alert callback functions **********

AlertCallbackType = Callable[[car.CarParams, car.CarState, messaging.SubMaster, bool, int], Alert]


def soft_disable_alert(alert_text_2: str) -> AlertCallbackType:
  def func(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
    if soft_disable_time < int(0.5 / DT_CTRL):
      return ImmediateDisableAlert(alert_text_2)
    return SoftDisableAlert(alert_text_2)
  return func

def user_soft_disable_alert(alert_text_2: str) -> AlertCallbackType:
  def func(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
    if soft_disable_time < int(0.5 / DT_CTRL):
      return ImmediateDisableAlert(alert_text_2)
    return UserSoftDisableAlert(alert_text_2)
  return func

def startup_master_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  branch = get_short_branch("")  # Ensure get_short_branch is cached to avoid lags on startup
  if "REPLAY" in os.environ:
    branch = "replay"

  return StartupAlert(_("WARNING: This branch is not tested"), branch, alert_status=AlertStatus.userPrompt)

def below_engage_speed_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  return NoEntryAlert(f"Drive above {get_display_speed(CP.minEnableSpeed, metric)} to engage")


def below_steer_speed_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  return Alert(
    _("Steer Unavailable Below %s") % get_display_speed(CP.minSteerSpeed, metric),
    "",
    AlertStatus.userPrompt, AlertSize.small,
    Priority.MID, VisualAlert.steerRequired, AudibleAlert.prompt, 0.4)


def calibration_incomplete_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  first_word = _('Recalibration') if sm['liveCalibration'].calStatus == log.LiveCalibrationData.Status.recalibrating else _('Calibration')
  return Alert(
    f"{first_word} in Progress: {sm['liveCalibration'].calPerc:.0f}%",
    f"Drive Above {get_display_speed(MIN_SPEED_FILTER, metric)}",
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .2)


def no_gps_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  return Alert(
    _("Poor GPS reception"),
    _("Hardware malfunctioning if sky is visible"),
    AlertStatus.normal, AlertSize.mid,
    Priority.LOWER, VisualAlert.none, AudibleAlert.none, .2, creation_delay=300.)

# *** debug alerts ***

def out_of_space_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  full_perc = round(100. - sm['deviceState'].freeSpacePercent)
  return NormalPermanentAlert(_("Out of Storage"), _("%s%% full") % full_perc)


def posenet_invalid_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  mdl = sm['modelV2'].velocity.x[0] if len(sm['modelV2'].velocity.x) else math.nan
  err = CS.vEgo - mdl
  msg = f"Speed Error: {err:.1f} m/s"
  return NoEntryAlert(msg, alert_text_1=_("Posenet Speed Invalid"))


def process_not_running_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  not_running = [p.name for p in sm['managerState'].processes if not p.running and p.shouldBeRunning]
  msg = ', '.join(not_running)
  return NoEntryAlert(msg, alert_text_1=_("Process Not Running"))


def comm_issue_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  bs = [s for s in sm.data.keys() if not sm.all_checks([s, ])]
  msg = ', '.join(bs[:4])  # can't fit too many on one line
  return NoEntryAlert(msg, alert_text_1=_("Communication Issue Between Processes"))


def camera_malfunction_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  all_cams = ('roadCameraState', 'driverCameraState', 'wideRoadCameraState')
  bad_cams = [s.replace('State', '') for s in all_cams if s in sm.data.keys() and not sm.all_checks([s, ])]
  return NormalPermanentAlert(_("Camera Malfunction"), ', '.join(bad_cams))


def calibration_invalid_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  rpy = sm['liveCalibration'].rpyCalib
  yaw = math.degrees(rpy[2] if len(rpy) == 3 else math.nan)
  pitch = math.degrees(rpy[1] if len(rpy) == 3 else math.nan)
  angles = f"Remount Device (Pitch: {pitch:.1f}°, Yaw: {yaw:.1f}°)"
  return NormalPermanentAlert(_("Calibration Invalid"), angles)


def overheat_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  cpu = max(sm['deviceState'].cpuTempC, default=0.)
  gpu = max(sm['deviceState'].gpuTempC, default=0.)
  temp = max((cpu, gpu, sm['deviceState'].memoryTempC))
  return NormalPermanentAlert(_("System Overheated"), f"{temp:.0f} °C")


def low_memory_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  return NormalPermanentAlert(_("Low Memory"), f"{sm['deviceState'].memoryUsagePercent}% used")


def high_cpu_usage_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  x = max(sm['deviceState'].cpuUsagePercent, default=0.)
  return NormalPermanentAlert(_("High CPU Usage"), _("%s%% used") % x)


def modeld_lagging_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  return NormalPermanentAlert(_("Driving Model Lagging"), f"{sm['modelV2'].frameDropPerc:.1f}% frames dropped")


def wrong_car_mode_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  text = _("Enable Adaptive Cruise to Engage")
  if CP.carName == "honda":
    text = _("Enable Main Switch to Engage")
  return NoEntryAlert(text)


def joystick_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  axes = sm['testJoystick'].axes
  gb, steer = list(axes)[:2] if len(axes) else (0., 0.)
  vals = f"Gas: {round(gb * 100.)}%, Steer: {round(steer * 100.)}%"
  return NormalPermanentAlert(_("Joystick Mode"), vals)

def speed_limit_adjust_alert(CP: car.CarParams, CS: car.CarState, sm: messaging.SubMaster, metric: bool, soft_disable_time: int) -> Alert:
  speedLimit = sm['longitudinalPlan'].speedLimit
  speed = round(speedLimit * (CV.MS_TO_KPH if metric else CV.MS_TO_MPH))
  message = _("Adjusting to %(speed)s %(unit)s") % ({"speed": speed, "unit": (_("km/h") if metric else _("mph"))})
  return Alert(
    message,
    "",
    AlertStatus.normal, AlertSize.small,
    Priority.LOW, VisualAlert.none, AudibleAlert.none, 4.)


EVENTS: Dict[int, Dict[str, Union[Alert, AlertCallbackType]]] = {
  # ********** events with no alerts **********

  EventName.stockFcw: {},

  # ********** events only containing alerts displayed in all states **********

  EventName.joystickDebug: {
    ET.WARNING: joystick_alert,
    ET.PERMANENT: NormalPermanentAlert(_("Joystick Mode")),
  },

  EventName.controlsInitializing: {
    ET.NO_ENTRY: NoEntryAlert(_("System Initializing")),
  },

  EventName.startup: {
    ET.PERMANENT: StartupAlert(_("Be ready to take over at any time"))
  },

  EventName.startupMaster: {
    ET.PERMANENT: startup_master_alert,
  },

  # Car is recognized, but marked as dashcam only
  EventName.startupNoControl: {
    ET.PERMANENT: StartupAlert(_("Dashcam mode")),
    ET.NO_ENTRY: NoEntryAlert(_("Dashcam mode")),
  },

  # Car is not recognized
  EventName.startupNoCar: {
    ET.PERMANENT: StartupAlert(_("Dashcam mode for unsupported car")),
  },

  EventName.startupNoFw: {
    ET.PERMANENT: StartupAlert(_("Car Unrecognized"),
                               _("Check comma power connections"),
                               alert_status=AlertStatus.userPrompt),
  },

  EventName.dashcamMode: {
    ET.PERMANENT: NormalPermanentAlert(_("Dashcam Mode"),
                                       priority=Priority.LOWEST),
  },

  EventName.invalidLkasSetting: {
    ET.PERMANENT: NormalPermanentAlert(_("Stock LKAS is on"),
                                       _("Turn off stock LKAS to engage")),
  },

  EventName.cruiseMismatch: {
    #ET.PERMANENT: ImmediateDisableAlert(_("openpilot failed to cancel cruise")),
  },

  # openpilot doesn't recognize the car. This switches openpilot into a
  # read-only mode. This can be solved by adding your fingerprint.
  # See https://github.com/commaai/openpilot/wiki/Fingerprinting for more information
  EventName.carUnrecognized: {
    ET.PERMANENT: NormalPermanentAlert(_("Dashcam Mode"),
                                       _("Car Unrecognized"),
                                       priority=Priority.LOWEST),
  },

  EventName.stockAeb: {
    ET.PERMANENT: Alert(
      _("BRAKE!"),
      _("Stock AEB: Risk of Collision"),
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.none, 2.),
    ET.NO_ENTRY: NoEntryAlert(_("Stock AEB: Risk of Collision")),
  },

  EventName.fcw: {
    ET.PERMANENT: Alert(
      _("BRAKE!"),
      _("Risk of Collision"),
      AlertStatus.critical, AlertSize.full,
      Priority.HIGHEST, VisualAlert.fcw, AudibleAlert.warningSoft, 2.),
  },

  EventName.ldw: {
    ET.PERMANENT: Alert(
      _("Lane Departure Detected"),
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.ldw, AudibleAlert.prompt, 3.),
  },

  # ********** events only containing alerts that display while engaged **********

  # openpilot tries to learn certain parameters about your car by observing
  # how the car behaves to steering inputs from both human and openpilot driving.
  # This includes:
  # - steer ratio: gear ratio of the steering rack. Steering angle divided by tire angle
  # - tire stiffness: how much grip your tires have
  # - angle offset: most steering angle sensors are offset and measure a non zero angle when driving straight
  # This alert is thrown when any of these values exceed a sanity check. This can be caused by
  # bad alignment or bad sensor data. If this happens consistently consider creating an issue on GitHub
  EventName.vehicleModelInvalid: {
    ET.NO_ENTRY: NoEntryAlert(_("Vehicle Parameter Identification Failed")),
    ET.SOFT_DISABLE: soft_disable_alert(_("Vehicle Parameter Identification Failed")),
  },

  EventName.steerTempUnavailableSilent: {
    ET.WARNING: Alert(
      _("Steering Temporarily Unavailable"),
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.prompt, 1.8),
  },

  EventName.preDriverDistracted: {
    ET.WARNING: Alert(
      _("Pay Attention"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.promptDriverDistracted: {
    ET.WARNING: Alert(
      _("Pay Attention"),
      _("Driver Distracted"),
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.promptDistracted, .1),
  },

  EventName.driverDistracted: {
    ET.WARNING: Alert(
      _("DISENGAGE IMMEDIATELY"),
      _("Driver Distracted"),
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.warningImmediate, .1),
  },

  EventName.preDriverUnresponsive: {
    ET.WARNING: Alert(
      _("Touch Steering Wheel: No Face Detected"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.promptDriverUnresponsive: {
    ET.WARNING: Alert(
      _("Touch Steering Wheel"),
      _("Driver Unresponsive"),
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.MID, VisualAlert.steerRequired, AudibleAlert.promptDistracted, .1),
  },

  EventName.driverUnresponsive: {
    ET.WARNING: Alert(
      _("DISENGAGE IMMEDIATELY"),
      _("Driver Unresponsive"),
      AlertStatus.critical, AlertSize.full,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.warningImmediate, .1),
  },

  EventName.manualRestart: {
    ET.WARNING: Alert(
      _("TAKE CONTROL"),
      _("Resume Driving Manually"),
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .2),
  },

  EventName.resumeRequired: {
    ET.WARNING: Alert(
      _("Press Resume to Exit Standstill"),
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .2),
  },

  EventName.belowSteerSpeed: {
    ET.WARNING: below_steer_speed_alert,
  },

  EventName.preLaneChangeLeft: {
    ET.PERMANENT: Alert(
      _("Steer Left to Start Lane Change Once Safe"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.preLaneChangeRight: {
    ET.PERMANENT: Alert(
      _("Steer Right to Start Lane Change Once Safe"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1, alert_rate=0.75),
  },

  EventName.laneChangeBlocked: {
    ET.PERMANENT: Alert(
      _("Car Detected in Blindspot or RoadEdge"),
      "",
      AlertStatus.userPrompt, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.prompt, .2),
  },

  EventName.laneChange: {
    ET.PERMANENT: Alert(
      _("Changing Lanes"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.steerSaturated: {
    ET.PERMANENT: Alert(
      _("Take Control"),
      _("Turn Exceeds Steering Limit"),
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.LOW, VisualAlert.steerRequired, AudibleAlert.promptRepeat, 1.),
  },

  # Thrown when the fan is driven at >50% but is not rotating
  EventName.fanMalfunction: {
    ET.PERMANENT: NormalPermanentAlert(_("Fan Malfunction"), _("Likely Hardware Issue")),
  },

  # Camera is not outputting frames
  EventName.cameraMalfunction: {
    ET.PERMANENT: camera_malfunction_alert,
    ET.SOFT_DISABLE: soft_disable_alert(_("Camera Malfunction")),
    ET.NO_ENTRY: NoEntryAlert(_("Camera Malfunction: Reboot Your Device")),
  },
  # Camera framerate too low
  EventName.cameraFrameRate: {
    ET.PERMANENT: NormalPermanentAlert(_("Camera Frame Rate Low"), _("Reboot your Device")),
    ET.SOFT_DISABLE: soft_disable_alert(_("Camera Frame Rate Low")),
    ET.NO_ENTRY: NoEntryAlert(_("Camera Frame Rate Low: Reboot Your Device")),
  },

  # Unused
  EventName.gpsMalfunction: {
    ET.PERMANENT: NormalPermanentAlert(_("GPS Malfunction"), _("Likely Hardware Issue")),
  },

  # When the GPS position and localizer diverge the localizer is reset to the
  # current GPS position. This alert is thrown when the localizer is reset
  # more often than expected.
  EventName.localizerMalfunction: {
    # ET.PERMANENT: NormalPermanentAlert(_("Sensor Malfunction"), _("Hardware Malfunction")),
  },

  EventName.speedLimitActive: {
    ET.WARNING: Alert(
      "Cruise set to speed limit",
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 2.),
  },

  EventName.speedLimitValueChange: {
    ET.WARNING: speed_limit_adjust_alert,
  },

  # ********** events that affect controls state transitions **********

  EventName.pcmEnable: {
    ET.ENABLE: EngagementAlert(AudibleAlert.engage),
  },

  EventName.buttonEnable: {
    ET.ENABLE: EngagementAlert(AudibleAlert.engage),
  },

  EventName.pcmDisable: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
  },

  EventName.buttonCancel: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert("Cancel Pressed"),
  },

  EventName.brakeHold: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert(_("Brake Hold Active")),
  },

  EventName.parkBrake: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert(_("Parking Brake Engaged")),
  },

  EventName.pedalPressed: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert(_("Pedal Pressed"),
                              visual_alert=VisualAlert.brakePressed),
  },

  EventName.preEnableStandstill: {
    ET.PRE_ENABLE: Alert(
      _("Release Brake to Engage"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1, creation_delay=1.),
  },

  EventName.gasPressedOverride: {
    ET.OVERRIDE_LONGITUDINAL: Alert(
      "",
      "",
      AlertStatus.normal, AlertSize.none,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.steerOverride: {
    ET.OVERRIDE_LATERAL: Alert(
      "",
      "",
      AlertStatus.normal, AlertSize.none,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .1),
  },

  EventName.wrongCarMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: wrong_car_mode_alert,
  },

  EventName.resumeBlocked: {
    ET.NO_ENTRY: NoEntryAlert("Press Set to Engage"),
  },

  EventName.wrongCruiseMode: {
    ET.USER_DISABLE: EngagementAlert(AudibleAlert.disengage),
    ET.NO_ENTRY: NoEntryAlert(_("Adaptive Cruise Disabled")),
  },

  EventName.steerTempUnavailable: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Steering Temporarily Unavailable")),
    ET.NO_ENTRY: NoEntryAlert(_("Steering Temporarily Unavailable")),
  },

  EventName.steerTimeLimit: {
    ET.SOFT_DISABLE: soft_disable_alert("Vehicle Steering Time Limit"),
    ET.NO_ENTRY: NoEntryAlert("Vehicle Steering Time Limit"),
  },

  EventName.outOfSpace: {
    ET.PERMANENT: out_of_space_alert,
    ET.NO_ENTRY: NoEntryAlert(_("Out of Storage")),
  },

  EventName.belowEngageSpeed: {
    ET.NO_ENTRY: below_engage_speed_alert,
  },

  EventName.sensorDataInvalid: {
    ET.PERMANENT: Alert(
      _("Sensor Data Invalid"),
      _("Ensure device is mounted securely"),
      AlertStatus.normal, AlertSize.mid,
      Priority.LOWER, VisualAlert.none, AudibleAlert.none, .2, creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert(_("Sensor Data Invalid")),
    ET.SOFT_DISABLE: soft_disable_alert(_("Sensor Data Invalid")),
  },

  EventName.noGps: {
    ET.PERMANENT: no_gps_alert,
  },

  EventName.soundsUnavailable: {
    ET.PERMANENT: NormalPermanentAlert(_("Speaker not found"), _("Reboot your Device")),
    ET.NO_ENTRY: NoEntryAlert(_("Speaker not found")),
  },

  EventName.tooDistracted: {
    ET.NO_ENTRY: NoEntryAlert(_("Distraction Level Too High")),
  },

  EventName.overheat: {
    ET.PERMANENT: overheat_alert,
    ET.SOFT_DISABLE: soft_disable_alert(_("System Overheated")),
    ET.NO_ENTRY: NoEntryAlert(_("System Overheated")),
  },

  EventName.wrongGear: {
    # ET.SOFT_DISABLE: user_soft_disable_alert(_("Gear not D")),
    ET.NO_ENTRY: NoEntryAlert(_("Gear not D")),
  },

  # This alert is thrown when the calibration angles are outside of the acceptable range.
  # For example if the device is pointed too much to the left or the right.
  # Usually this can only be solved by removing the mount from the windshield completely,
  # and attaching while making sure the device is pointed straight forward and is level.
  # See https://comma.ai/setup for more information
  EventName.calibrationInvalid: {
    ET.PERMANENT: calibration_invalid_alert,
    ET.SOFT_DISABLE: soft_disable_alert(_("Calibration Invalid: Remount Device & Recalibrate")),
    ET.NO_ENTRY: NoEntryAlert(_("Calibration Invalid: Remount Device & Recalibrate")),
  },

  EventName.calibrationIncomplete: {
    ET.PERMANENT: calibration_incomplete_alert,
    ET.SOFT_DISABLE: soft_disable_alert("Calibration Incomplete"),
    ET.NO_ENTRY: NoEntryAlert("Calibration in Progress"),
  },

  EventName.calibrationRecalibrating: {
    ET.PERMANENT: calibration_incomplete_alert,
    ET.SOFT_DISABLE: soft_disable_alert("Device Remount Detected: Recalibrating"),
    ET.NO_ENTRY: NoEntryAlert("Remount Detected: Recalibrating"),
  },

  EventName.doorOpen: {
    ET.SOFT_DISABLE: user_soft_disable_alert(_("Door Open")),
    ET.NO_ENTRY: NoEntryAlert(_("Door Open")),
  },

  EventName.seatbeltNotLatched: {
    ET.SOFT_DISABLE: user_soft_disable_alert(_("Seatbelt Unlatched")),
    ET.NO_ENTRY: NoEntryAlert(_("Seatbelt Unlatched")),
  },

  EventName.espDisabled: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Electronic Stability Control Disabled")),
    ET.NO_ENTRY: NoEntryAlert(_("Electronic Stability Control Disabled")),
  },

  EventName.lowBattery: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Low Battery")),
    ET.NO_ENTRY: NoEntryAlert(_("Low Battery")),
  },

  # Different openpilot services communicate between each other at a certain
  # interval. If communication does not follow the regular schedule this alert
  # is thrown. This can mean a service crashed, did not broadcast a message for
  # ten times the regular interval, or the average interval is more than 10% too high.
  EventName.commIssue: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Communication Issue between Processes")),
    ET.NO_ENTRY: comm_issue_alert,
  },
  EventName.commIssueAvgFreq: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Low Communication Rate between Processes")),
    ET.NO_ENTRY: NoEntryAlert(_("Low Communication Rate between Processes")),
  },

  EventName.controlsdLagging: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Controls Lagging")),
    ET.NO_ENTRY: NoEntryAlert(_("Controls Process Lagging: Reboot Your Device")),
  },

  # Thrown when manager detects a service exited unexpectedly while driving
  EventName.processNotRunning: {
    ET.NO_ENTRY: process_not_running_alert,
    ET.SOFT_DISABLE: soft_disable_alert(_("Process Not Running")),
  },

  EventName.radarFault: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Radar Error: Restart the Car")),
    ET.NO_ENTRY: NoEntryAlert(_("Radar Error: Restart the Car")),
  },

  # Every frame from the camera should be processed by the model. If modeld
  # is not processing frames fast enough they have to be dropped. This alert is
  # thrown when over 20% of frames are dropped.
  EventName.modeldLagging: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Driving Model Lagging")),
    ET.NO_ENTRY: NoEntryAlert(_("Driving Model Lagging")),
    ET.PERMANENT: modeld_lagging_alert,
  },

  # Besides predicting the path, lane lines and lead car data the model also
  # predicts the current velocity and rotation speed of the car. If the model is
  # very uncertain about the current velocity while the car is moving, this
  # usually means the model has trouble understanding the scene. This is used
  # as a heuristic to warn the driver.
  EventName.posenetInvalid: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Posenet Speed Invalid")),
    ET.NO_ENTRY: posenet_invalid_alert,
  },

  # When the localizer detects an acceleration of more than 40 m/s^2 (~4G) we
  # alert the driver the device might have fallen from the windshield.
  EventName.deviceFalling: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Device Fell Off Mount")),
    ET.NO_ENTRY: NoEntryAlert(_("Device Fell Off Mount")),
  },

  EventName.lowMemory: {
    ET.SOFT_DISABLE: soft_disable_alert(_("Low Memory: Reboot Your Device")),
    ET.PERMANENT: low_memory_alert,
    ET.NO_ENTRY: NoEntryAlert(_("Low Memory: Reboot Your Device")),
  },

  EventName.highCpuUsage: {
    #ET.SOFT_DISABLE: soft_disable_alert(_("System Malfunction: Reboot Your Device")),
    #ET.PERMANENT: NormalPermanentAlert(_("System Malfunction"), _("Reboot your Device")),
    ET.NO_ENTRY: high_cpu_usage_alert,
  },

  EventName.accFaulted: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("Cruise Fault: Restart the Car")),
    ET.PERMANENT: NormalPermanentAlert(_("Cruise Fault: Restart the car to engage")),
    ET.NO_ENTRY: NoEntryAlert(_("Cruise Fault: Restart the Car")),
  },

  EventName.controlsMismatch: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("Controls Mismatch")),
    ET.NO_ENTRY: NoEntryAlert(_("Controls Mismatch")),
  },

  EventName.roadCameraError: {
    ET.PERMANENT: NormalPermanentAlert(_("Camera CRC Error - Road"),
                                       duration=1.,
                                       creation_delay=30.),
  },

  EventName.wideRoadCameraError: {
    ET.PERMANENT: NormalPermanentAlert(_("Camera CRC Error - Road Fisheye"),
                                       duration=1.,
                                       creation_delay=30.),
  },

  EventName.driverCameraError: {
    ET.PERMANENT: NormalPermanentAlert(_("Camera CRC Error - Driver"),
                                       duration=1.,
                                       creation_delay=30.),
  },

  # Sometimes the USB stack on the device can get into a bad state
  # causing the connection to the panda to be lost
  EventName.usbError: {
    ET.SOFT_DISABLE: soft_disable_alert(_("USB Error: Reboot Your Device")),
    ET.PERMANENT: NormalPermanentAlert(_("USB Error: Reboot Your Device"), ""),
    ET.NO_ENTRY: NoEntryAlert(_("USB Error: Reboot Your Device")),
  },

  # This alert can be thrown for the following reasons:
  # - No CAN data received at all
  # - CAN data is received, but some message are not received at the right frequency
  # If you're not writing a new car port, this is usually cause by faulty wiring
  EventName.canError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("CAN Error")),
    ET.PERMANENT: Alert(
      _("CAN Error: Check Connections"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 1., creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert(_("CAN Error: Check Connections")),
  },

  EventName.canBusMissing: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("CAN Bus Disconnected")),
    ET.PERMANENT: Alert(
      _("CAN Bus Disconnected: Likely Faulty Cable"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, 1., creation_delay=1.),
    ET.NO_ENTRY: NoEntryAlert(_("CAN Bus Disconnected: Check Connections")),
  },

  EventName.steerUnavailable: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("LKAS Fault: Restart the Car")),
    ET.PERMANENT: NormalPermanentAlert(_("LKAS Fault: Restart the car to engage")),
    ET.NO_ENTRY: NoEntryAlert(_("LKAS Fault: Restart the Car")),
  },

  EventName.reverseGear: {
    ET.PERMANENT: Alert(
      _("Reverse\nGear"),
      "",
      AlertStatus.normal, AlertSize.none,
      Priority.LOWEST, VisualAlert.none, AudibleAlert.none, .2, creation_delay=0.5),
    # ET.USER_DISABLE: ImmediateDisableAlert(_("Reverse Gear")),
    ET.NO_ENTRY: NoEntryAlert(_("Reverse Gear")),
  },

  # On cars that use stock ACC the car can decide to cancel ACC for various reasons.
  # When this happens we can no long control the car so the user needs to be warned immediately.
  EventName.cruiseDisabled: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("Cruise Is Off")),
  },

  # For planning the trajectory Model Predictive Control (MPC) is used. This is
  # an optimization algorithm that is not guaranteed to find a feasible solution.
  # If no solution is found or the solution has a very high cost this alert is thrown.
  EventName.plannerError: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("Planner Solution Error")),
    ET.NO_ENTRY: NoEntryAlert(_("Planner Solution Error")),
  },

  # When the relay in the harness box opens the CAN bus between the LKAS camera
  # and the rest of the car is separated. When messages from the LKAS camera
  # are received on the car side this usually means the relay hasn't opened correctly
  # and this alert is thrown.
  EventName.relayMalfunction: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("Harness Relay Malfunction")),
    ET.PERMANENT: NormalPermanentAlert(_("Harness Relay Malfunction"), _("Check Hardware")),
    ET.NO_ENTRY: NoEntryAlert(_("Harness Relay Malfunction")),
  },

  EventName.speedTooLow: {
    ET.IMMEDIATE_DISABLE: Alert(
      _("openpilot Canceled"),
      _("Speed too low"),
      AlertStatus.normal, AlertSize.mid,
      Priority.HIGH, VisualAlert.none, AudibleAlert.disengage, 3.),
  },

  # When the car is driving faster than most cars in the training data, the model outputs can be unpredictable.
  EventName.speedTooHigh: {
    ET.WARNING: Alert(
      _("Speed Too High"),
      _("Model uncertain at this speed"),
      AlertStatus.userPrompt, AlertSize.mid,
      Priority.HIGH, VisualAlert.steerRequired, AudibleAlert.promptRepeat, 4.),
    ET.NO_ENTRY: NoEntryAlert(_("Slow down to engage")),
  },

  EventName.lowSpeedLockout: {
    ET.PERMANENT: NormalPermanentAlert(_("Cruise Fault: Restart the car to engage")),
    ET.NO_ENTRY: NoEntryAlert(_("Cruise Fault: Restart the Car")),
  },

  EventName.lkasDisabled: {
    ET.PERMANENT: NormalPermanentAlert(_("LKAS Disabled: Enable LKAS to engage")),
    ET.NO_ENTRY: NoEntryAlert(_("LKAS Disabled")),
  },

  EventName.vehicleSensorsInvalid: {
    ET.IMMEDIATE_DISABLE: ImmediateDisableAlert(_("Vehicle Sensors Invalid")),
    ET.PERMANENT: NormalPermanentAlert(_("Vehicle Sensors Calibrating"), _("Drive to Calibrate")),
    ET.NO_ENTRY: NoEntryAlert(_("Vehicle Sensors Calibrating"), _("Drive to Calibrate")),
  },

  # dp - use for manual lane change
  EventName.manualSteeringRequiredBlinkersOn: {
    ET.PERMANENT: Alert(
      _("STEERING REQUIRED: Blinkers ON"),
      "",
      AlertStatus.normal, AlertSize.small,
      Priority.LOW, VisualAlert.none, AudibleAlert.none, .0, alert_rate=0.25),
  },
}
