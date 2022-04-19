How to install on Oneplus 3t?
------
1. clone openpilot to /data/:
   (在 ssh 畫面下，輸入)
```
cd /data/ && rm -fr openpilot ; git clone https://github.com/eFiniLan/openpilot.git openpilot -b master
```

2. run command:
   (在 ssh 畫面下，輸入)
```
cd /data/openpilot && git submodule init && git submodule update
cd /data/openpilot/scripts/ && ./oneplus_update_neos.sh
```

3. Let it download and complete it update, after a couple of reboot, your screen will then stay in fastboot mode.
   (等待下載並讓它重新開機，沒錯誤的話會進入 Android 機器人更新畫面，等自動重新開機)

4. In fastboot mode, select use volume button to select to `Recovery mode` then press power button.
   (在 fastboot 模式，用音量鍵上下選到 Recovery mode 再按下電源鍵)

5. In Recovery mode, tap `apply update` -> `Choose from emulated` -> `0/` -> `update.zip` -> `Reboot system now`
   (在 Recovery mode，點選 `apply update` -> `Choose from emulated` -> `0/` -> `update.zip` -> `Reboot system now`)

6. You should be able to boot into openpilot, if touch screen is not working, try to reboot again.
   (你現在應該可以進入 openpilot 畫面，如果點擊畫面沒有反應，請再重新開機一次)


The MIT License
------
Copyright (c) 2019-, Rick Lan, dragonpilot community, and a number of other of contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
