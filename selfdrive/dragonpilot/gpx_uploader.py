#!/usr/bin/env python3
# The MIT License
#
# Copyright (c) 2019-, Rick Lan, dragonpilot community, and a number of other of contributors.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import os
import time
from common.params import Params
from system.version import get_version

# for uploader
from system.loggerd.xattr_cache import getxattr, setxattr
import glob
import requests
import json

# customisable values
GPX_LOG_PATH = '/data/media/0/gpx_logs/'
LOG_HERTZ = 1/10 # 0.1 Hz = 10 sec, higher for higher accuracy, 10hz seems fine

# uploader
UPLOAD_ATTR_NAME = 'user.upload'
UPLOAD_ATTR_VALUE = b'1'
LOG_PATH = '/data/media/0/gpx_logs/'

# osm api
API_HEADER = {'Authorization': 'Bearer 2pvUyXfk9vizuh7PwQFSEYBtFWcM-Pu7vxApUjSA0fc'}
VERSION_URL = 'https://api.openstreetmap.org/api/versions'
UPLOAD_URL = 'https://api.openstreetmap.org/api/0.6/gpx/create'

_DEBUG = False

def _debug(msg):
  if not _DEBUG:
    return
  print(msg, flush=True)

class GpxUploader():
  def __init__(self):
    self._delete_after_upload = not Params().get_bool('dp_gpxd')
    self._car_model = "Unknown Vehicle"

  def _identify_vehicle(self):
    # read model from LiveParameters
    params = Params().get("LiveParameters")
    if params is not None:
      params = json.loads(params)
      self._car_model = params.get('carFingerprint', self._car_model)
    self._dp_version = get_version()
    _debug("GpxUploader init - _delete_after_upload = %s" % self._delete_after_upload)
    _debug("GpxUploader init - _car_model = %s" % self._car_model)

  def _is_online(self):
    try:
      r = requests.get(VERSION_URL, headers=API_HEADER)
      _debug("is_online? status_code = %s" % r.status_code)
      return r.status_code >= 200
    except Exception:
      return False

  def _get_is_uploaded(self, filename):
    result = getxattr(filename, UPLOAD_ATTR_NAME) is not None
    _debug("%s is uploaded: %s" % (filename, result))
    return result

  def _set_is_uploaded(self, filename):
    _debug("%s set to uploaded" % filename)
    setxattr(filename, UPLOAD_ATTR_NAME, UPLOAD_ATTR_VALUE)

  def _get_files(self):
    return sorted( filter( os.path.isfile, glob.glob(LOG_PATH + '*') ) )

  def _get_files_to_be_uploaded(self):
    files = self._get_files()
    files_to_be_uploaded = []
    for file in files:
      if not self._get_is_uploaded(file):
        files_to_be_uploaded.append(file)
    return files_to_be_uploaded

  def _do_upload(self, filename):
    fn = os.path.basename(filename)
    data = {
      'description': "Routes from dragonpilot %s (%s)." % (self._dp_version, self._car_model),
      'visibility': 'identifiable'
    }
    files = {
      "file": (fn, open(filename, 'rb'))
    }
    try:
      r = requests.post(UPLOAD_URL, files=files, data=data, headers=API_HEADER)
      _debug("do_upload - %s - %s" % (filename, r.status_code))
      return r.status_code == 200
    except Exception:
      return False

  def run(self):
    # give it few seconds before we start runing the process
    # only identify vehicle once
    time.sleep(10)
    self._identify_vehicle()
    while True:
      is_offroad = Params().get_bool("IsOffroad")
      files = self._get_files_to_be_uploaded()
      if len(files) == 0:
        if is_offroad and self._delete_after_upload:
          for file in self._get_files():
            os.remove(file)
        _debug("run - no files, clean stash")
      elif not self._is_online() and self._delete_after_upload:
        _debug("run - not online & delete_after_upload")
        for file in files:
          os.remove(file)
      else:
        for file in files:
          if self._do_upload(file):
            if self._delete_after_upload:
              _debug("run - _delete_after_upload")
              os.remove(file)
            else:
              _debug("run - set_is_uploaded")
              self._set_is_uploaded(file)
      time.sleep(60)

def gpx_uploader_thread():
  gpx_uploader = GpxUploader()
  gpx_uploader.run()

def main():
  gpx_uploader_thread()

if __name__ == "__main__":
  main()
