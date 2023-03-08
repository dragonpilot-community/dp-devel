#!/usr/bin/env python3
import argparse
import multiprocessing
import rpyc # pylint: disable=import-error
from collections import defaultdict

from helper import download_rinex, exec_LimeGPS_bin
from helper import get_random_coords, get_continuous_coords

#------------------------------------------------------------------------------
# this script is supposed to run on HOST PC
# limeSDR is unreliable via c3 USB
#------------------------------------------------------------------------------


def run_lime_gps(rinex_file: str, location: str, timeout: int):
  # needs to run longer than the checker
  timeout += 10
  print(f"LimeGPS {location} {timeout}")
  p = multiprocessing.Process(target=exec_LimeGPS_bin,
                              args=(rinex_file, location, timeout))
  p.start()
  return p

con = None
def run_remote_checker(lat, lon, alt, duration, ip_addr):
  global con
  try:
    con = rpyc.connect(ip_addr, 18861)
    con._config['sync_request_timeout'] = duration+20
  except ConnectionRefusedError:
    print("could not run remote checker is 'rpc_server.py' running???")
    return False, None, None

def get_random_coords(lat, lon) -> Tuple[int, int]:
  # jump around the world
  # max values, lat: -90 to 90, lon: -180 to 180

  lat_add = random.random()*20 + 10
  lon_add = random.random()*20 + 20

  lat = ((lat + lat_add + 90) % 180) - 90
  lon = ((lon + lon_add + 180) % 360) - 180
  return round(lat, 5), round(lon, 5)

def get_continuous_coords(lat, lon) -> Tuple[int, int]:
  # continuously move around the world

  lat_add = random.random()*0.01
  lon_add = random.random()*0.01

  lat = ((lat + lat_add + 90) % 180) - 90
  lon = ((lon + lon_add + 180) % 360) - 180
  return round(lat, 5), round(lon, 5)

rc_p: Any = None
def exec_remote_checker(lat, lon, duration, ip_addr):
  global rc_p
  # TODO: good enough for testing
  remote_cmd =  "export PYTHONPATH=/data/pythonpath:/data/pythonpath/pyextra && "
  remote_cmd += "cd /data/openpilot && "
  remote_cmd += f"timeout {duration} /usr/local/pyenv/shims/python tools/gpstest/remote_checker.py "
  remote_cmd += f"{lat} {lon}"

  ssh_cmd = ["ssh", "-i", "/home/batman/openpilot/xx/phone/key/id_rsa",
             f"comma@{ip_addr}"]
  ssh_cmd += [remote_cmd]

  rc_p = sp.Popen(ssh_cmd, stdout=sp.PIPE)
  rc_p.wait()
  rc_output = rc_p.stdout.read()
  print(f"Checker Result: {rc_output.strip().decode('utf-8')}")

  print(f"Remote Checker: {log} {info}")
  return matched, log, info


stats = defaultdict(int) # type: ignore
keys = ['success', 'failed', 'ublox_fail', 'laikad_fail', 'proc_crash', 'checker_crash']

def print_report():
  print("\nFuzzy testing report summary:")
  for k in keys:
    print(f"  {k}: {stats[k]}")


def update_stats(matched, log, info):
  if matched:
    stats['success'] += 1
    return

  stats['failed'] += 1
  if log == "PROC CRASH":
    stats['proc_crash'] += 1
  if log == "CHECKER CRASHED":
    stats['checker_crash'] += 1
  if log == "TIMEOUT":
    if "LAIKAD" in info:
      stats['laikad_fail'] += 1
    else: # "UBLOX" in info
      stats['ublox_fail'] += 1


def main(ip_addr, continuous_mode, timeout, pos):
  rinex_file = download_rinex()

  lat, lon, alt = pos
  if lat == 0 and lon == 0 and alt == 0:
    lat, lon, alt = get_random_coords(47.2020, 15.7403)

  try:
    while True:
      # spoof random location
      spoof_proc = run_lime_gps(rinex_file, f"{lat},{lon},{alt}", timeout)

      # remote checker execs blocking
      matched, log, info = run_remote_checker(lat, lon, alt, timeout, ip_addr)
      update_stats(matched, log, info)
      spoof_proc.terminate()
      spoof_proc = None

      if continuous_mode:
        lat, lon, alt = get_continuous_coords(lat, lon, alt)
      else:
        lat, lon, alt = get_random_coords(lat, lon)
  except KeyboardInterrupt:
    if spoof_proc is not None:
      spoof_proc.terminate()

    if con is not None and not con.closed:
      con.root.exposed_kill_procs()
      con.close()

    print_report()


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="Fuzzy test GPS stack with random locations.")
  parser.add_argument("ip_addr", type=str)
  parser.add_argument("-c", "--contin", type=bool, nargs='?', default=False, help='Continous location change')
  parser.add_argument("-t", "--timeout", type=int, nargs='?', default=180, help='Timeout to get location')

  # for replaying a location
  parser.add_argument("lat", type=float, nargs='?', default=0)
  parser.add_argument("lon", type=float, nargs='?', default=0)
  parser.add_argument("alt", type=float, nargs='?', default=0)
  args = parser.parse_args()
  main(args.ip_addr, args.contin, args.timeout, (args.lat, args.lon, args.alt))
