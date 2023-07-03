"""Install exception handler for process crash."""
import sentry_sdk
from enum import Enum
from sentry_sdk.integrations.threading import ThreadingIntegration

from common.params import Params
# from selfdrive.athena.registration import is_registered_device
from system.hardware import HARDWARE, PC
from system.swaglog import cloudlog
from system.version import get_branch, get_commit, get_origin, get_version, \
                              is_comma_remote, is_dirty, is_tested_branch

import os
import traceback
import requests
from cereal import car
from datetime import datetime

class SentryProject(Enum):
  # python project
  SELFDRIVE = "https://980a0cba712a4c3593c33c78a12446e1@o273754.ingest.sentry.io/1488600"
  # native project
  SELFDRIVE_NATIVE = "https://980a0cba712a4c3593c33c78a12446e1@o273754.ingest.sentry.io/1488600"

CRASHES_DIR = '/data/media/0/crash_logs'

ret = car.CarParams.new_message()
candidate = ret.carFingerprint

params = Params()
#uniqueID = op_params.get('uniqueID')
try:
  dongle_id = params.get("DongleId").decode('utf8')
except AttributeError:
  dongle_id = "None"
try:
  gitname = Params().get("GithubUsername", encoding='utf-8')
except Exception:
  gitname = ""
error_tags = {'dirty': is_dirty(), 'dongle_id': dongle_id, 'branch': get_branch(), 'remote': get_origin(), 'fingerprintedAs': candidate, 'gitname':gitname}

try:
  cached_params = params.get("CarParams")
  if cached_params is not None:
    cached_params = car.CarParams.from_bytes(cached_params)
    car_name = cached_params.carFingerprint
  else:
    car_name = "None"
except Exception:
  car_name = "None"

def report_tombstone(fn: str, message: str, contents: str) -> None:
  cloudlog.error({'tombstone': message})

  with sentry_sdk.configure_scope() as scope:
    scope.set_extra("tombstone_fn", fn)
    scope.set_extra("tombstone", contents)
    sentry_sdk.capture_message(message=message)
    sentry_sdk.flush()


def capture_exception(*args, **kwargs) -> None:
  save_exception(traceback.format_exc())
  cloudlog.error("crash", exc_info=kwargs.get('exc_info', 1))

  try:
    sentry_sdk.capture_exception(*args, **kwargs)
    sentry_sdk.flush()  # https://github.com/getsentry/sentry-python/issues/291
  except Exception:
    cloudlog.exception("sentry exception")

def save_exception(exc_text):
  if not os.path.exists(CRASHES_DIR):
    os.makedirs(CRASHES_DIR)

  log_file = '{}/{}'.format(CRASHES_DIR, datetime.now().strftime('%Y-%m-%d-%H-%M-%S.log'))
  with open(log_file, 'w') as f:
    f.write(exc_text)
  print('Logged current crash to {}'.format(log_file))

def bind_user(**kwargs) -> None:
    sentry_sdk.set_user(kwargs)

def capture_warning(warning_string):
  bind_user(id=dongle_id, name=gitname)
  sentry_sdk.capture_message(warning_string, level='warning')

def capture_info(info_string):
  bind_user(id=dongle_id, name=gitname)
  sentry_sdk.capture_message(info_string, level='info')

def get_ip_address():
  try:
    ip = requests.get('https://checkip.amazonaws.com/').text.strip()
  except Exception:
    ip = "255.255.255.255"
  return ip

def set_tag(key: str, value: str) -> None:
  sentry_sdk.set_tag(key, value)

def before_send_event(event, hint):
  if "user" in event and "ip_address" in event["user"]:
    event["user"]["ip_address"] = "[Filtered]"  # Replace the IP address with "[Filtered]"
  return event



def init(project: SentryProject) -> None:
  # forks like to mess with this, so double check
  #comma_remote = is_comma_remote() and "commaai" in get_origin(default="")
  #if not comma_remote or not is_registered_device() or PC:
    #return

  env = "release" if is_tested_branch() else "master"
  dongle_id = Params().get("DongleId", encoding='utf-8')
  gitname = Params().get("GithubUsername", encoding='utf-8')
  ip = get_ip_address()

  integrations = []
  if project == SentryProject.SELFDRIVE:
    integrations.append(ThreadingIntegration(propagate_hub=True))
  else:
    sentry_sdk.utils.MAX_STRING_LENGTH = 8192

  sentry_sdk.init(project.value,
                  default_integrations=False,
                  release=get_version(),
                  integrations=integrations,
                  traces_sample_rate=1.0,
                  environment=env,
                  send_default_pii=True,  # Enable sending personally identifiable information (PII)
                  before_send=before_send_event,  # Attach a custom before_send event handler
                )

  with sentry_sdk.configure_scope() as scope:
    scope.set_user({"id": dongle_id, "gitname": gitname})
    scope.set_tag("dirty", is_dirty())
    scope.set_tag("origin", get_origin())
    scope.set_tag("branch", get_branch())
    scope.set_tag("commit", get_commit())
    scope.set_tag("device", HARDWARE.get_device_type())
    scope.set_tag("model", car_name)
    scope.set_extra("ip_address", ip)

  if project == SentryProject.SELFDRIVE:
    sentry_sdk.Hub.current.start_session()
