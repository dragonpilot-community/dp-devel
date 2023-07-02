#!/usr/bin/env python3
from enum import Enum

class LongTunes(Enum):
  TSS2 = 0
  TSS = 1

class LatTunes(Enum):
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


###### LONG ######
def set_long_tune(tune, name):
  # Improved longitudinal tune
  if name == LongTunes.TSS2:
    tune.deadzoneBP = [0., 8.05]
    tune.deadzoneV = [.0, .14]
    tune.kpBP = [0., 5., 20., 30.]
    tune.kpV = [1.3, 1.0, 0.7, 0.1]
    #really smooth (make it toggleable)
    #tune.kiBP = [0., 0.07, 5, 8, 11., 18., 20., 24., 33.]
    #tune.kiV = [.001, .01, .1, .18, .21, .22, .23, .22, .001]
    #okay ish
    #tune.kiBP = [0., 11., 17., 20., 24., 30., 33., 40.]
    #tune.kiV = [.001, .21, .22, .23, .22, .1, .001, .0001]
    tune.kiBP = [0.,   6.,  8.,  11., 30., 33., 40.]
    tune.kiV = [.001, .07,  .15, .2,  .2,  .01, .0001]
  # Default longitudinal tune
  elif name == LongTunes.TSS:
    tune.deadzoneBP = [0., 9.]
    tune.deadzoneV = [.0, .15]
    tune.kpBP = [0., 5., 35.]
    tune.kiBP = [0., 35.]
    tune.kpV = [3.6, 2.4, 1.5]
    tune.kiV = [0.54, 0.36]
  else:
    raise NotImplementedError('This longitudinal tune does not exist')


###### LAT ######
def set_lat_tune(tune, name, MAX_LAT_ACCEL=2.5, FRICTION=0.01, steering_angle_deadzone_deg=0.0, use_steering_angle=True):

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
    else:
      raise NotImplementedError('This PID tune does not exist')
  else:
    raise NotImplementedError('This lateral tune does not exist')
