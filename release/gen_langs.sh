#!/usr/bin/env sh
xgettext -d base -o ../selfdrive/assets/locales/events.pot ../selfdrive/controls/lib/events.py
msgmerge --update ../selfdrive/assets/locales/zh-TW/LC_MESSAGES/events.po ../selfdrive/assets/locales/events.pot
msgfmt -o ../selfdrive/assets/locales/zh-TW/LC_MESSAGES/events.mo ../selfdrive/assets/locales/zh-TW/LC_MESSAGES/events

lupdate ../selfdrive/ui/ui.pro
linguist ../selfdrive/ui/translations/zh-TW.ts
lrelease ../selfdrive/ui/ui.pro