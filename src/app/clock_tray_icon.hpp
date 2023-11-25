#pragma once

#include <QSystemTrayIcon>

#include <QTime>

class ClockTrayIcon : public QSystemTrayIcon
{
  Q_OBJECT
public:
  explicit ClockTrayIcon(QObject* parent = nullptr);

private slots:
  void updateIcon();

private:
  QTime m_last_update;
};
