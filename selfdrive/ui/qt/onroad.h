#pragma once

#include <QPushButton>
#include <QStackedLayout>
#include <QWidget>

#include "common/util.h"
#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/widgets/cameraview.h"


const int btn_size = 192;
const int img_size = (btn_size / 4) * 3;


// ***** onroad widgets *****
class OnroadAlerts : public QWidget {
  Q_OBJECT

public:
  OnroadAlerts(QWidget *parent = 0) : QWidget(parent) {};
  void updateAlert(const Alert &a, const QColor &color);

protected:
  void paintEvent(QPaintEvent*) override;

private:
  QColor bg;
  Alert alert = {};
};

class ExperimentalButton : public QPushButton {
  Q_OBJECT

public:
  explicit ExperimentalButton(QWidget *parent = 0);
  void updateState(const UIState &s);

private:
  void paintEvent(QPaintEvent *event) override;

  Params params;
  QPixmap engage_img;
  QPixmap experimental_img;
};

// container window for the NVG UI
class AnnotatedCameraWidget : public CameraWidget {
  Q_OBJECT
  Q_PROPERTY(float speed MEMBER speed);
  Q_PROPERTY(QString speedUnit MEMBER speedUnit);
  Q_PROPERTY(float setSpeed MEMBER setSpeed);
  Q_PROPERTY(float speedLimitComma MEMBER speedLimitComma);
  Q_PROPERTY(bool is_cruise_set MEMBER is_cruise_set);
  Q_PROPERTY(bool has_eu_speed_limit MEMBER has_eu_speed_limit);
  Q_PROPERTY(bool has_us_speed_limit MEMBER has_us_speed_limit);
  Q_PROPERTY(bool is_metric MEMBER is_metric);

  Q_PROPERTY(bool dmActive MEMBER dmActive);
  Q_PROPERTY(bool hideDM MEMBER hideDM);
  Q_PROPERTY(bool rightHandDM MEMBER rightHandDM);
  Q_PROPERTY(int status MEMBER status);
  //dp
  Q_PROPERTY(bool showVTC MEMBER showVTC);
  Q_PROPERTY(QString vtcSpeed MEMBER vtcSpeed);
  Q_PROPERTY(QColor vtcColor MEMBER vtcColor);
  Q_PROPERTY(bool showDebugUI MEMBER showDebugUI);

  Q_PROPERTY(QString roadName MEMBER roadName);

  Q_PROPERTY(bool showSpeedLimit MEMBER showSpeedLimit);
  Q_PROPERTY(float speedLimit MEMBER speedLimit);
  Q_PROPERTY(QString slcSubText MEMBER slcSubText);
  Q_PROPERTY(float slcSubTextSize MEMBER slcSubTextSize);
  Q_PROPERTY(bool mapSourcedSpeedLimit MEMBER mapSourcedSpeedLimit);
  Q_PROPERTY(bool slcActive MEMBER slcActive);

  Q_PROPERTY(bool showTurnSpeedLimit MEMBER showTurnSpeedLimit);
  Q_PROPERTY(QString turnSpeedLimit MEMBER turnSpeedLimit);
  Q_PROPERTY(QString mtscSubText MEMBER mtscSubText);
  Q_PROPERTY(bool mtscActive MEMBER mtscActive);
  Q_PROPERTY(int curveSign MEMBER curveSign);

public:
  explicit AnnotatedCameraWidget(VisionStreamType type, QWidget* parent = 0);
  void updateState(const UIState &s);

private:
  void drawIcon(QPainter &p, int x, int y, QPixmap &img, QBrush bg, float opacity);
  void drawText(QPainter &p, int x, int y, const QString &text, int alpha = 255);
  void drawCenteredText(QPainter &p, int x, int y, const QString &text, QColor color);
  void drawVisionTurnControllerUI(QPainter &p, int x, int y, int size, const QColor &color, const QString &speed,
                                  int alpha);
  void drawCircle(QPainter &p, int x, int y, int r, QBrush bg);
  void drawTurnSpeedSign(QPainter &p, QRect rc, const QString &speed, const QString &sub_text, int curv_sign, bool is_active);

  ExperimentalButton *experimental_btn;
  QPixmap dm_img;
  QPixmap map_img;
  QPixmap left_img;
  QPixmap right_img;
  const int radius = 192;
  const int img_size = (radius / 2) * 1.5;
  const int subsign_img_size = 35;
  float speed;
  QString speedUnit;
  float setSpeed;
  float speedLimitComma;
  bool is_cruise_set = false;
  bool is_metric = false;
  bool dmActive = false;
  bool hideDM = false;
  bool rightHandDM = false;
  float dm_fade_state = 1.0;
  bool has_us_speed_limit = false;
  bool has_eu_speed_limit = false;
  bool v_ego_cluster_seen = false;
  int status = STATUS_DISENGAGED;
  std::unique_ptr<PubMaster> pm;
  //dp
  bool showVTC = false;
  QString vtcSpeed;
  QColor vtcColor;

  bool showDebugUI = false;

  QString roadName;

  bool showSpeedLimit = false;
  float speedLimit;
  QString slcSubText;
  float slcSubTextSize = 0.0;
  bool mapSourcedSpeedLimit = false;
  bool slcActive = false;

  bool showTurnSpeedLimit = false;
  QString turnSpeedLimit;
  QString mtscSubText;
  bool mtscActive = false;
  int curveSign = 0;

  int skip_frame_count = 0;
  bool wide_cam_requested = false;

protected:
  void paintGL() override;
  void initializeGL() override;
  void showEvent(QShowEvent *event) override;
  void updateFrameMat() override;
  void drawLaneLines(QPainter &painter, const UIState *s);
  void drawLead(QPainter &painter, const cereal::RadarState::LeadData::Reader &lead_data, const QPointF &vd);
  void drawHud(QPainter &p);
  void drawDriverState(QPainter &painter, const UIState *s);
  inline QColor redColor(int alpha = 255) { return QColor(201, 34, 49, alpha); }
  inline QColor whiteColor(int alpha = 255) { return QColor(255, 255, 255, alpha); }
  inline QColor blackColor(int alpha = 255) { return QColor(0, 0, 0, alpha); }

  double prev_draw_t = 0;
  FirstOrderFilter fps_filter;
};

// container for all onroad widgets
class OnroadWindow : public QWidget {
  Q_OBJECT

public:
  OnroadWindow(QWidget* parent = 0);
  bool isMapVisible() const { return map && map->isVisible(); }

private:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent* e) override;
  OnroadAlerts *alerts;
  AnnotatedCameraWidget *nvg;
  QColor bg = bg_colors[STATUS_DISENGAGED];
  QWidget *map = nullptr;
  QHBoxLayout* split;

private slots:
  void offroadTransition(bool offroad);
  void updateState(const UIState &s);
};
