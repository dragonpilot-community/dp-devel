#!/usr/bin/env python3
from selfdrive.car import gen_empty_fingerprint


global saved_candidate
saved_candidate = {}
global saved_finger
saved_finger = gen_empty_fingerprint()
global saved_CarInterface
global enabled
enabled = False
