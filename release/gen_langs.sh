#!/bin/bash
xgettext -d base -o ../selfdrive/assets/locales/events.pot ../selfdrive/controls/lib/events.py
declare -a langs=("zh-TW" "zh-CN" "ja-JP" "ko-KR")

for i in "${langs[@]}"
do
  msgmerge --update ../selfdrive/assets/locales/$i/LC_MESSAGES/events.po ../selfdrive/assets/locales/events.pot
  msgfmt -o ../selfdrive/assets/locales/$i/LC_MESSAGES/events.mo ../selfdrive/assets/locales/$i/LC_MESSAGES/events
done
