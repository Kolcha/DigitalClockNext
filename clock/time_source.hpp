#pragma once

#include <QObject>

#include <QDateTime>
#include <QTimer>

class TimeSource : public QObject
{
  Q_OBJECT

public:
  explicit TimeSource(QObject* parent = nullptr)
    : QObject(parent)
  {
    connect(&_timer, &QTimer::timeout, this, &TimeSource::onTimeout);
    using namespace std::chrono_literals;
    _timer.start(500ms);
  }

  ~TimeSource()
  {
    _timer.stop();
  }

  QDateTime now() const { return QDateTime::currentDateTimeUtc(); }

signals:
  // provides current UTC time, interval is unspecified
  void timeChanged(const QDateTime& dt);
  // provides stable flag changes every 0.5 second
  void halfSecondUpdate(bool flag);

private slots:
  void onTimeout()
  {
    emit timeChanged(now());
    // this assumes 0.5s timer interval, reimplement on change
    _flag = !_flag;
    emit halfSecondUpdate(_flag);
  }

private:
  QTimer _timer;
  bool _flag = true;
};
