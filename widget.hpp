#pragma once

#include <QWidget>

#include <memory>

class QTimer;
class ClockWidget;

class Widget : public QWidget
{
  Q_OBJECT

public:
  Widget(QWidget *parent = nullptr);
  ~Widget();

protected:
  void paintEvent(QPaintEvent* e) override;

private:
  std::unique_ptr<ClockWidget> _clock;
  QTimer* _timer;
};
