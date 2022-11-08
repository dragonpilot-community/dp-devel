#!/usr/bin/env python3
from cereal import car
from common.params import Params
from common.realtime import Priority, config_realtime_process
from system.swaglog import cloudlog
from selfdrive.controls.lib.longitudinal_planner import LongitudinalPlanner
from selfdrive.controls.lib.lateral_planner import LateralPlanner
import cereal.messaging as messaging

from selfdrive.dragonpilot.controls_0813.lib.lateral_planner import LateralPlanner as DPLateralPlanner
from selfdrive.dragonpilot.controls_0816.lib.lateral_planner import LateralPlanner as FPLateralPlanner

def plannerd_thread(sm=None, pm=None):
  config_realtime_process(5, Priority.CTRL_LOW)

  cloudlog.info("plannerd is waiting for CarParams")
  params = Params()
  CP = car.CarParams.from_bytes(params.get("CarParams", block=True))
  cloudlog.info("plannerd got CarParams: %s", CP.carName)

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
    lateral_planner = LateralPlanner(CP)

  if sm is None:
    sm = messaging.SubMaster(['carControl', 'carState', 'controlsState', 'radarState', 'modelV2', 'dragonConf', 'lateralPlan', 'navInstruction', 'liveMapData'],
                             poll=['radarState', 'modelV2'], ignore_avg_freq=['radarState'])

  if pm is None:
    pm = messaging.PubMaster(['longitudinalPlan', 'lateralPlan'])

  while True:
    sm.update()

    if sm.updated['modelV2']:
      lateral_planner.update(sm)
      lateral_planner.publish(sm, pm)
      longitudinal_planner.update(sm)
      longitudinal_planner.publish(sm, pm)


def main(sm=None, pm=None):
  plannerd_thread(sm, pm)


if __name__ == "__main__":
  main()
