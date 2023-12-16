#pragma once

#include <QObject>

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QVersionNumber>

class UpdateChecker : public QObject
{
  Q_OBJECT

public:
  explicit UpdateChecker(QDateTime last_update, QObject* parent = nullptr);

  QDateTime lastUpdateTime() const noexcept;

signals:
  void newVersion(QVersionNumber version, QDate date, QUrl link);
  void upToDate();
  void errorMessage(QString msg);
  void updateChecked(QDateTime last_update);

public slots:
  void checkForUpdates();
  void setCheckForBeta(bool check) noexcept;
  void setAutoupdate(bool update) noexcept;
  void setUpdatePeriod(int period) noexcept;
  void timerHandler();

private:
  void runCheckForUpdates(bool force);
  void sendRequest(const QUrl& url);
  void readReply(QNetworkReply* reply);

private:
  bool _autoupdate;
  bool _check_beta;
  bool _force_update;
  bool _is_running;

  int _update_period;   // in days

  QNetworkAccessManager _net_access_manager;
  QDateTime _last_update;
};
