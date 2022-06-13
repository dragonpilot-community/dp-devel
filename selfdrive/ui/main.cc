#include <sys/resource.h>

#include <QApplication>

#include "system/hardware/hw.h"
#include "selfdrive/ui/qt/qt_window.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/window.h"
#include <QTranslator>

int main(int argc, char *argv[]) {
  setpriority(PRIO_PROCESS, 0, -20);

  qInstallMessageHandler(swagLogMessageHandler);
  initApp(argc, argv);

  QApplication a(argc, argv);

  QString locale = "en-US";
  locale = QString::fromStdString(Params().get("dp_locale"));
  QTranslator translator;
  // fall back to English
  if (!translator.load(locale, "translations")) {
    translator.load("en-US", "translations");
  }
  a.installTranslator(&translator);  // needs to be before setting main window

  MainWindow w;
  setMainWindow(&w);
  a.installEventFilter(&w);
  return a.exec();
}
