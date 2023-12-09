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

#include "http_client.hpp"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace digital_clock {
namespace core {

HttpClient::HttpClient(QString user_agent, QObject* parent)
  : QObject(parent)
  , user_agent_(std::move(user_agent))
  , reply_(nullptr)
  , is_running_(false)
{
}

HttpClient::~HttpClient()
{
  if (is_running_) cancel();
}

void HttpClient::startRequest(QUrl url)
{
  url_ = std::move(url);
  is_running_ = true;
  QNetworkRequest req(url_);
  req.setHeader(QNetworkRequest::UserAgentHeader, user_agent_);
  reply_ = qnam_.get(req);
  connect(reply_, &QNetworkReply::finished, this, &HttpClient::httpFinished);
  connect(reply_, &QNetworkReply::readyRead, this, &HttpClient::httpReadyRead);
}

bool HttpClient::isRunning() const
{
  return is_running_;
}

void HttpClient::cancel()
{
  if (is_running_) reply_->abort();
}

bool HttpClient::hasError() const
{
  return reply_ && reply_->error();
}

void HttpClient::httpFinished()
{
  QVariant redirectionTarget = reply_->attribute(QNetworkRequest::RedirectionTargetAttribute);
  if (reply_->error()) {
    emit error(reply_->errorString());
  } else if (!redirectionTarget.isNull()) {
    // redirect to provided url
    url_ = url_.resolved(redirectionTarget.toUrl());
    reply_->deleteLater();
    startRequest(url_);
    return;
  } else {
    // download was successful, process data
    emit finished();
  }

  reply_->deleteLater();
  reply_ = nullptr;
  is_running_ = false;
}

void HttpClient::httpReadyRead()
{
  // this slot gets called every time the QNetworkReply has new data.
  if (reply_->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull()) {
    emit downloaded(reply_->readAll());
  }
}

} // namespace core
} // namespace digital_clock
