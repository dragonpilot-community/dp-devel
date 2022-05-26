#!/usr/bin/env python3.7
# import subprocess
# from cereal import car
from common.params import Params
from common.realtime import sec_since_boot
import os
params = Params()
LAST_MODIFIED = params.get_param_path() + "/dp_last_modified"

# delay of reading last modified
# LAST_MODIFIED_TIMER_THERMALD = 10.
LAST_MODIFIED_TIMER_SYSTEMD = 1.
# LAST_MODIFIED_TIMER_LANE_PLANNER = 3.
# LAST_MODIFIED_TIMER_UPLOADER = 10.

# def is_online():
#   try:
#     return not subprocess.call(["ping", "-W", "4", "-c", "1", "117.28.245.92"])
#   except ProcessLookupError:
#     return False
#
# def common_controller_ctrl(enabled, dragonconf, blinker_on, steer_req, v_ego):
#   if enabled:
#     if dragonconf.dpLateralMode == 0 and blinker_on:
#       steer_req = 0 if isinstance(steer_req, int) else False
#   return steer_req
#
def get_last_modified(delay, old_check, old_modified):
  new_check = sec_since_boot()
  if os.path.isfile(LAST_MODIFIED) and (old_check is None or new_check - old_check >= delay):
    return new_check, os.stat(LAST_MODIFIED).st_mtime
  else:
    return old_check, old_modified

# def param_get_if_updated(param, type, old_val, old_modified):
#   try:
#     modified = os.stat(PARAM_PATH + param).st_mtime
#   except OSError:
#     return old_val, old_modified
#   if old_modified != modified:
#     new_val = param_get(param, type, old_val)
#     new_modified = modified
#   else:
#     new_val = old_val
#     new_modified = old_modified
#   return new_val, new_modified

# def param_get(param_name, type, default):
#   try:
#     val = params.get(param_name, encoding='utf8').rstrip('\x00')
#     if type == 'bool':
#       val = val == '1'
#     elif type == 'int':
#       val = int(val)
#     elif type == 'float':
#       val = float(val)
#   except (TypeError, ValueError):
#     val = default
#   return val
