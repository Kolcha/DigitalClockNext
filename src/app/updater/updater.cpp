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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "updater.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

#include "build_date.hpp"
#include "http_client.hpp"

namespace digital_clock {
namespace core {

Updater::Updater(QDateTime last_update, QObject* parent)
  : QObject(parent)
  , autoupdate_(true)
  , check_beta_(false)
  , force_update_(false)
  , update_period_(3)
  , last_update_(std::move(last_update))
{
  auto ua = QString("%1/%2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion());
  downloader_ = new HttpClient(std::move(ua), this);
  connect(downloader_, &HttpClient::error, this, &Updater::ErrorMessage);
  connect(downloader_, &HttpClient::downloaded, this, [this] (const QByteArray& data) { data_.append(data); });
  connect(downloader_, &HttpClient::finished, this, &Updater::ProcessData);
}

Updater::~Updater()
{
  if (downloader_->isRunning()) downloader_->cancel();
}

QDateTime Updater::GetLastUpdateTime() const
{
  return last_update_;
}

void Updater::CheckForUpdates()
{
  RunCheckForUpdates(true);
}

void Updater::SetCheckForBeta(bool check)
{
  check_beta_ = check;
}

void Updater::SetAutoupdate(bool update)
{
  autoupdate_ = update;
}

void Updater::SetUpdatePeriod(int period)
{
  update_period_ = period;
}

void Updater::TimeoutHandler()
{
  if (!autoupdate_ || downloader_->isRunning())
    return;

  if (last_update_.daysTo(QDateTime::currentDateTime()) < update_period_)
    return;

  RunCheckForUpdates(false);
}

void Updater::ProcessData()
{
  if (downloader_->hasError()) return;

  QJsonParseError err;
  QJsonDocument js_doc = QJsonDocument::fromJson(data_, &err);
  if (err.error != QJsonParseError::NoError) {
    emit ErrorMessage(err.errorString());
    return;
  }

  QJsonObject js_obj = js_doc.object().value("stable").toObject();
  QVersionNumber latest = QVersionNumber::fromString(js_obj.value("version").toString());
  QDate last_build = QDate::fromString(js_obj.value("timestamp").toString().simplified(), "dd-MM-yyyy");
  QString link = js_obj.value("download").toString();

  if (check_beta_) {
    js_obj = js_doc.object().value("testing").toObject();
    QString t_version = js_obj.value("version").toString("-");
    if (t_version != "-") {
      latest = QVersionNumber::fromString(t_version);
      last_build = QDate::fromString(js_obj.value("timestamp").toString().simplified(), "dd-MM-yyyy");
      link = js_obj.value("download").toString();
    }
  }

  if (latest > QVersionNumber::fromString(QCoreApplication::applicationVersion()) || build_date() < last_build) {
    emit NewVersion(latest, last_build, {link});
  } else {
    if (force_update_) emit UpToDate();
  }

  data_.clear();
  emit UpdateChecked(last_update_);
}

void Updater::RunCheckForUpdates(bool force)
{
  force_update_ = force;
  data_.clear();
  last_update_ = QDateTime::currentDateTime();
  downloader_->startRequest(QUrl("https://digitalclock4.sourceforge.io/update/"));
}

} // namespace core
} // namespace digital_clock
