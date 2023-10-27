/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2013-2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QObject>

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QVersionNumber>

namespace digital_clock {
namespace core {

class HttpClient;

class Updater : public QObject
{
  Q_OBJECT

public:
  explicit Updater(QDateTime last_update, QObject* parent = nullptr);
  ~Updater();

  QDateTime GetLastUpdateTime() const;

signals:
  void NewVersion(QVersionNumber version, QDate date, QUrl link);
  void UpToDate();
  void ErrorMessage(QString msg);
  void UpdateChecked(QDateTime last_update);

public slots:
  void CheckForUpdates();
  void SetCheckForBeta(bool check);
  void SetAutoupdate(bool update);
  void SetUpdatePeriod(int period);
  void TimeoutHandler();

private slots:
  void ProcessData();

private:
  void RunCheckForUpdates(bool force);

private:
  bool autoupdate_;
  bool check_beta_;
  bool force_update_;

  int update_period_;   // in days

  HttpClient* downloader_;
  QByteArray data_;
  QDateTime last_update_;
};

} // namespace core
} // namespace digital_clock
