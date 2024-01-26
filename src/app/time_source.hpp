/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

private slots:
  void onTimeout()
  {
    emit timeChanged(now());
  }

private:
  QTimer _timer;
};
