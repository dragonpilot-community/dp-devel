HEADERS         = ui.h \
                  qt/widgets/drive_stats.h

SOURCES         = main.cc \
                  qt/home.cc \
                  qt/onroad.cc \
                  qt/sidebar.cc \
                  qt/util.cc \

                  qt/maps/map.cc \
                  qt/maps/map_settings.cc \

                  qt/offroad/driverview.cc \
                  qt/offroad/networking.cc \
                  qt/offroad/onboarding.cc \
                  qt/offroad/settings.cc \

                  qt/widgets/drive_stats.cc \
                  qt/widgets/input.cc \
                  qt/widgets/offroad_alerts.cc \
                  qt/widgets/prime.cc \
                  qt/widgets/ssh_keys.cc \
                  qt/widgets/setup.cc

TRANSLATIONS    = translations/en-US.ts \
                  translations/zh-TW.ts \
                  translations/zh-CN.ts
