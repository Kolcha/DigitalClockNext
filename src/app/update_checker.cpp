#include "update_checker.hpp"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "build_date.hpp"

UpdateChecker::UpdateChecker(QDateTime last_update, QObject* parent)
  : QObject(parent)
    , _autoupdate(true)
    , _check_beta(false)
    , _force_update(false)
    , _is_running(false)
    , _update_period(3)
    , _last_update(std::move(last_update))
{
  connect(&_net_access_manager, &QNetworkAccessManager::finished, this, &UpdateChecker::readReply);
}

QDateTime UpdateChecker::lastUpdateTime() const noexcept
{
  return _last_update;
}

void UpdateChecker::checkForUpdates()
{
  runCheckForUpdates(true);
}

void UpdateChecker::setCheckForBeta(bool check) noexcept
{
  _check_beta = check;
}

void UpdateChecker::setAutoupdate(bool update) noexcept
{
  _autoupdate = update;
}

void UpdateChecker::setUpdatePeriod(int period) noexcept
{
  _update_period = period;
}

void UpdateChecker::timerHandler()
{
  if (!_autoupdate || _is_running)
    return;

  if (_last_update.daysTo(QDateTime::currentDateTime()) < _update_period)
    return;

  runCheckForUpdates(false);
}

void UpdateChecker::runCheckForUpdates(bool force)
{
  _force_update = force;
  _last_update = QDateTime::currentDateTime();
  sendRequest(QUrl("https://digitalclock4.sourceforge.io/update/"));
}

void UpdateChecker::sendRequest(const QUrl& url)
{
  _is_running = true;
  auto ua = QString("%1/%2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion());
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::UserAgentHeader, ua);
  _net_access_manager.get(req);
}

void UpdateChecker::readReply(QNetworkReply* reply)
{
  _is_running = false;
  reply->deleteLater();

  if (reply->error()) {
    emit errorMessage(reply->errorString());
    return;
  }

  QByteArray data = reply->readAll();

  QJsonParseError err;
  QJsonDocument js_doc = QJsonDocument::fromJson(data, &err);
  if (err.error != QJsonParseError::NoError) {
    emit errorMessage(err.errorString());
    return;
  }

  QJsonObject js_obj = js_doc.object().value("stable").toObject();
  QVersionNumber latest = QVersionNumber::fromString(js_obj.value("version").toString());
  QDate last_build = QDate::fromString(js_obj.value("timestamp").toString().simplified(), "dd-MM-yyyy");
  QString link = js_obj.value("download").toString();

  if (_check_beta) {
    js_obj = js_doc.object().value("testing").toObject();
    QString t_version = js_obj.value("version").toString("-");
    if (t_version != "-") {
      latest = QVersionNumber::fromString(t_version);
      last_build = QDate::fromString(js_obj.value("timestamp").toString().simplified(), "dd-MM-yyyy");
      link = js_obj.value("download").toString();
    }
  }

  if (latest > QVersionNumber::fromString(QCoreApplication::applicationVersion()) || build_date() < last_build) {
    emit newVersion(latest, last_build, {link});
  } else {
    if (_force_update) emit upToDate();
  }

  emit updateChecked(_last_update);
}
