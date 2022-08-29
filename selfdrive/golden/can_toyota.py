#!/usr/bin/env python3
import os
import cereal.messaging as messaging
from opendbc.can.packer import CANPacker
from selfdrive.boardd.boardd_api_impl import can_list_to_can_capnp  # pylint: disable=no-name-in-module,import-error
#from selfdrive.car.toyota.values import CAR
#from selfdrive.car import crc8_pedal
import math
from opendbc.can.parser import CANParser

SAFETY_MODE = 2
SAFETY_PARAM = 73

def get_car_can_parser():
  dbc_f = 'toyota_tnga_k_pt_generated'
  signals = [
  ]
  checks = [
  ]
  return CANParser(dbc_f, signals, checks, 0)

cp = get_car_can_parser()

packer = CANPacker("toyota_tnga_k_pt_generated")
rpacker = CANPacker("toyota_adas")

SR = 7.5

def angle_to_sangle(angle):
  return - math.degrees(angle) * SR

def can_function(pm, speed, angle, idx, cruise_button=0, is_engaged=False):
  msg = []

  msg.append(packer.make_can_msg("PCM_CRUISE_2", 0, {"SET_SPEED": speed, "MAIN_ON": 1 if is_engaged else 0}))
  msg.append(packer.make_can_msg("WHEEL_SPEEDS", 0,
    {"WHEEL_SPEED_FL": speed,
     "WHEEL_SPEED_FR": speed,
     "WHEEL_SPEED_RL": speed,
     "WHEEL_SPEED_RR": speed}))
  if os.path.exists('/tmp/op_lc'):
    msg.append(packer.make_can_msg("STEERING_LEVERS", 0, {"TURN_SIGNALS": 2}))

  msg.append(packer.make_can_msg("STEER_ANGLE_SENSOR", 0, {"STEER_ANGLE": angle_to_sangle(angle)}))
  msg.append(packer.make_can_msg("STEER_TORQUE_SENSOR", 0, {"STEER_ANGLE": angle_to_sangle(angle)}))
  msg.append(packer.make_can_msg("EPS_STATUS", 0, {"LKA_STATE": 5}))
  msg.append(packer.make_can_msg("GEAR_PACKET", 0, {"GEAR": 0}))

  msg.append(packer.make_can_msg("PCM_CRUISE", 0, {"CRUISE_ACTIVE": 1 if is_engaged else 0, "GAS_RELEASED": 1}))
  #print(msg)
  # cam bus
  msg.append(packer.make_can_msg("PRE_COLLISION", 2, {}))
  msg.append(packer.make_can_msg("STEERING_LKA", 2, {}))

  if idx % 10 == 0 :
    # radar
    i = 0
    for i in range(16):
      msg.append(rpacker.make_can_msg("TRACK_A_%d" % i, 1, {"VALID":60, "LONG_DIST": 255.5}))

    j = 0
    for j in range(16):
      msg.append(rpacker.make_can_msg("TRACK_B_%d" % j, 1, {"SCORE":100}))
  # fill in the rest for fingerprint
  #done = set([x[0] for x in msg])
  #for k, v in FINGERPRINTS[CAR.HIGHLANDER][0].items():
    #if k not in done and k not in [0xFFF]:
     # msg.append([k, 0, b'\x00'*v, 0])
  pm.send('can', can_list_to_can_capnp(msg))

def sendcan_function(sendcan):
  sc = messaging.drain_sock_raw(sendcan)
  cp.update_strings(sc, sendcan=True)
  brake = 0.0
  gas = 0.0
  steer_torque = 0.0

  # if cp.vl[0x1fa]['COMPUTER_BRAKE_REQUEST']:
  #   brake = cp.vl[0x1fa]['COMPUTER_BRAKE'] * 0.003906248
  # else:
  #   brake = 0.0
  #
  # if cp.vl[0x200]['GAS_COMMAND'] > 0:
  #   gas = cp.vl[0x200]['GAS_COMMAND'] / 256.0
  # else:
  #   gas = 0.0
  #
  # if cp.vl[0xe4]['STEER_TORQUE_REQUEST']:
  #   steer_torque = cp.vl[0xe4]['STEER_TORQUE']*1.0/0x1000
  # else:
  #   steer_torque = 0.0

  return (gas, brake, steer_torque)
