#!/usr/bin/env python3
from enum import Enum

class LatTunes(Enum):
  #TOYOTA
  INDI_PRIUS = 0
  LQR_RAV4 = 1
  PID_A = 2
  PID_B = 3
  PID_C = 4
  PID_D = 5
  PID_E = 6
  PID_F = 7
  PID_G = 8
  PID_I = 9
  PID_H = 10
  PID_J = 11
  PID_K = 12
  PID_L = 13
  PID_M = 14
  PID_N = 15
  INDI_PRIUS_TSS2 = 16

  #HKG
  PID_HYUNDAI_A = 17
  PID_HYUNDAI_B = 18
  PID_HYUNDAI_C = 19
  PID_HYUNDAI_D = 20
  PID_HYUNDAI_E = 21
  PID_HYUNDAI_F = 22
  PID_HYUNDAI_G = 23

  #VW
  PID_VW = 24

  #SUBARU
  PID_SUBARU_A = 25
  PID_SUBARU_B = 26
  PID_SUBARU_C = 27
  PID_SUBARU_D = 28
  PID_SUBARU_E = 29
  PID_SUBARU_F = 30

###### LAT ######
def set_lat_tune(tune, name, MAX_LAT_ACCEL=2.5, FRICTION=0.01, steering_angle_deadzone_deg=0.0, use_steering_angle=True):

  #TODO: add toggle for sepcial prius_tss2 indi
  if name == LatTunes.INDI_PRIUS_TSS2:
    tune.init('indi')
    #tune.indi.innerLoopGainBP = [20, 24, 30]
    #tune.indi.innerLoopGainV = [7.25, 7.5, 9]
    #tune.indi.outerLoopGainBP = [20, 24, 30]
    #tune.indi.outerLoopGainV = [6, 7.25, 6]
    #tune.indi.timeConstantBP = [20, 24]
    #tune.indi.timeConstantV = [2.0, 2.2]
    #tune.indi.actuatorEffectivenessBP = [20, 24]
    #tune.indi.actuatorEffectivenessV = [2, 3]
    tune.indi.innerLoopGainBP = [0.]
    tune.indi.innerLoopGainV = [15]
    tune.indi.outerLoopGainBP = [0.]
    tune.indi.outerLoopGainV = [17]
    tune.indi.timeConstantBP = [0.]
    tune.indi.timeConstantV = [4.5]
    tune.indi.actuatorEffectivenessBP = [0.]
    tune.indi.actuatorEffectivenessV = [15]
  elif 'PID' in str(name):
    tune.init('pid')
    tune.pid.kiBP = [0.0]
    tune.pid.kpBP = [0.0]
    if name == LatTunes.PID_A:
      tune.pid.kpV = [0.2]
      tune.pid.kiV = [0.05]
      tune.pid.kf = 0.00003
    elif name == LatTunes.PID_C:
      tune.pid.kpV = [0.6]
      tune.pid.kiV = [0.1]
      tune.pid.kf = 0.00006
    elif name == LatTunes.PID_D:
      tune.pid.kpV = [0.6]
      tune.pid.kiV = [0.1]
      tune.pid.kf = 0.00007818594
    elif name == LatTunes.PID_F:
      tune.pid.kpV = [0.723]
      tune.pid.kiV = [0.0428]
      tune.pid.kf = 0.00006
    elif name == LatTunes.PID_G:
      tune.pid.kpV = [0.18]
      tune.pid.kiV = [0.015]
      tune.pid.kf = 0.00012
    elif name == LatTunes.PID_H:
      tune.pid.kpV = [0.17]
      tune.pid.kiV = [0.03]
      tune.pid.kf = 0.00006
    elif name == LatTunes.PID_I:
      tune.pid.kpV = [0.15]
      tune.pid.kiV = [0.05]
      tune.pid.kf = 0.00004
    elif name == LatTunes.PID_J:
      tune.pid.kpV = [0.19]
      tune.pid.kiV = [0.02]
      tune.pid.kf = 0.00007818594
    elif name == LatTunes.PID_L:
      tune.pid.kpV = [0.3]
      tune.pid.kiV = [0.05]
      tune.pid.kf = 0.00006
    elif name == LatTunes.PID_M:
      tune.pid.kpV = [0.3]
      tune.pid.kiV = [0.05]
      tune.pid.kf = 0.00007
    elif name == LatTunes.PID_N:
      tune.pid.kpV = [0.35]
      tune.pid.kiV = [0.15]
      tune.pid.kf = 0.00007818594

    # hyundai
    elif name == LatTunes.PID_HYUNDAI_A:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[0.], [0.]]
      tune.pid.kpV, tune.pid.kiV = [[0.25], [0.05]]
    elif name == LatTunes.PID_HYUNDAI_B:
      tune.pid.kf = 0.00006
      tune.pid.kiBP, tune.pid.kpBP = [[0.], [0.]]
      tune.pid.kpV, tune.pid.kiV = [[0.25], [0.05]]
    elif name == LatTunes.PID_HYUNDAI_C:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[0.], [0.]]
      tune.pid.kpV, tune.pid.kiV = [[0.3], [0.05]]
    elif name == LatTunes.PID_HYUNDAI_D:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[9., 22.], [9., 22.]]
      tune.pid.kpV, tune.pid.kiV = [[0.2, 0.35], [0.05, 0.09]]
    elif name == LatTunes.PID_HYUNDAI_E:
      tune.pid.kf = 0.
      tune.pid.kiBP, tune.pid.kpBP = [[0.], [0.]]
      tune.pid.kpV, tune.pid.kiV = [[0.112], [0.004]]
    elif name == LatTunes.PID_HYUNDAI_F:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[0.], [0.]]
      tune.pid.kpV, tune.pid.kiV = [[0.16], [0.01]]
    elif name == LatTunes.PID_HYUNDAI_G:
      tune.pid.kiBP, tune.pid.kpBP = [[0.], [0.]]
      tune.pid.kpV, tune.pid.kiV = [[0.16], [0.01]]

    # vw
    elif name == LatTunes.PID_VW:
      tune.pid.kpBP = [0.]
      tune.pid.kiBP = [0.]
      tune.pid.kf = 0.00006
      tune.pid.kpV = [0.6]
      tune.pid.kiV = [0.2]

    # subaru
    elif name == LatTunes.PID_SUBARU_A:
      tune.pid.kf = 0.00003
      tune.pid.kiBP, tune.pid.kpBP = [[0., 20.], [0., 20.]]
      tune.pid.kpV, tune.pid.kiV = [[0.0025, 0.1], [0.00025, 0.01]]
    elif name == LatTunes.PID_SUBARU_B:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[0., 20.], [0., 20.]]
      tune.pid.kpV, tune.pid.kiV = [[0.2, 0.3], [0.02, 0.03]]
    elif name == LatTunes.PID_SUBARU_C:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[0., 14., 23.], [0., 14., 23.]]
      tune.pid.kpV, tune.pid.kiV = [[0.045, 0.042, 0.20], [0.04, 0.035, 0.045]]
    elif name == LatTunes.PID_SUBARU_D:
      tune.pid.kf = 0.000038
      tune.pid.kiBP, tune.pid.kpBP = [[0., 14., 23.], [0., 14., 23.]]
      tune.pid.kpV, tune.pid.kiV = [[0.01, 0.065, 0.2], [0.001, 0.015, 0.025]]
    elif name == LatTunes.PID_SUBARU_E:
      tune.pid.kf = 0.000039
      tune.pid.kiBP, tune.pid.kpBP = [[0., 10., 20.], [0., 10., 20.]]
      tune.pid.kpV, tune.pid.kiV = [[0.01, 0.05, 0.2], [0.003, 0.018, 0.025]]
    elif name == LatTunes.PID_SUBARU_F:
      tune.pid.kf = 0.00005
      tune.pid.kiBP, tune.pid.kpBP = [[0., 20.], [0., 20.]]
      tune.pid.kpV, tune.pid.kiV = [[0.1, 0.2], [0.01, 0.02]]

    else:
      raise NotImplementedError('This PID tune does not exist')
  else:
    raise NotImplementedError('This lateral tune does not exist')
