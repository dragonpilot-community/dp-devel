#!/usr/bin/env sh

DFU_UTIL="dfu-util"

DEFAULT_FW_FN="/data/openpilot/panda/board/obj/panda.bin.signed"
DEFAULT_SSPOOF_FW_FN="/data/openpilot/panda/board/obj/panda.bin.sspoof.signed"
TESTING_FW_FN="/data/openpilot/panda/board/obj/panda.bin.testing.signed"
TESTING_SSPOOF_FW_FN="/data/openpilot/panda/board/obj/panda.bin.testing.sspoof.signed"

ATL_ENABLED=0
ATL=`cat /data/params/d/dp_atl`
if [ -f /data/params/d/dp_atl ] && [ $ATL != "0" ]; then
  ATL_ENABLED=1
fi

SSPOOF_ENABLED=0
SSPOOF=`cat /data/params/d/dp_sspoof`
if [ -f /data/params/d/dp_sspoof ] && [ $SSPOOF != "0" ]; then
  SSPOOF_ENABLED=1
fi

FW=$DEFAULT_FW_FN
if [ "$ATL_ENABLED" = "1" ] && [ "$SSPOOF_ENABLED" = "1" ] && [ -f $TESTING_SSPOOF_FW_FN ]; then
  echo "Use testing + sspoof firmware..."
  FW=$TESTING_SSPOOF_FW_FN
elif [ "$ATL_ENABLED" = "1" ] && [ "$SSPOOF_ENABLED" = "0" ] && [ -f $TESTING_FW_FN ]; then
  echo "Use testing firmware..."
  FW=$TESTING_FW_FN
elif [ "$ATL_ENABLED" = "0" ] && [ "$SSPOOF_ENABLED" = "1" ] && [ -f $DEFAULT_SSPOOF_FW_FN ]; then
  echo "Use sspoof firmware..."
  FW=$DEFAULT_SSPOOF_FW_FN
else
  echo "Use stock firmware..."
fi

/data/openpilot/selfdrive/ui/qt/spinner &
pkill -f boardd
python -c "from panda import Panda; Panda().reset(enter_bootstub=True); Panda().reset(enter_bootloader=True)" || true
sleep 1
echo "\n\n\nUpdating panda.bin..."
$DFU_UTIL -d 0483:df11 -a 0 -s 0x08004000 -D $FW
echo "\n\n\nUpdating bootstub.panda.bin..."
$DFU_UTIL -d 0483:df11 -a 0 -s 0x08000000:leave -D /data/openpilot/panda/board/obj/bootstub.panda.bin
sleep 1
echo -n 1 > /data/params/d/DoReboot
