#!/usr/bin/env sh

DFU_UTIL="dfu-util"
/data/openpilot/selfdrive/ui/qt/spinner &
python -c "from panda import Panda; Panda().flash('/data/openpilot/panda/board/obj/panda.bin.signed')"
killall spinner &
