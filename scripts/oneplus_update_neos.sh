#!/usr/bin/bash

if [ -z "$BASEDIR" ]; then
  BASEDIR="/data/openpilot"
fi

source "$BASEDIR/launch_env.sh"
echo "Installing NEOS update"
NEOS_PY="$BASEDIR/selfdrive/hardware/eon/neos.py"
MANIFEST="$BASEDIR/selfdrive/hardware/eon/oneplus.json"
$NEOS_PY --swap-if-ready $MANIFEST
$BASEDIR/selfdrive/hardware/eon/updater $NEOS_PY $MANIFEST
