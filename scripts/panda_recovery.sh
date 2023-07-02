#!/usr/bin/env sh

DFU_UTIL="dfu-util"

DEFAULT_FW_FN="/data/openpilot/panda/board/obj/panda.bin.signed"

/data/openpilot/selfdrive/ui/qt/spinner &
pkill -f boardd
python -c "from panda import Panda; Panda().reset(enter_bootstub=True); Panda().reset(enter_bootloader=True)" || true
sleep 1
echo "\n\n\nUpdating panda.bin..."
$DFU_UTIL -d 0483:df11 -a 0 -s 0x08004000 -D $DEFAULT_FW_FN
echo "\n\n\nUpdating bootstub.panda.bin..."
$DFU_UTIL -d 0483:df11 -a 0 -s 0x08000000:leave -D /data/openpilot/panda/board/obj/bootstub.panda.bin
sleep 1
echo -n 1 > /data/params/d/DoReboot
