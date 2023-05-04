#pragma once

#include <QObject>
#include <QRegularExpression>
#include "datetime_formatter.hpp"

class LegacySkinFormatExtension final : public QObject,
                                        public DateTimeFormatter::Extension
{
  Q_OBJECT

public:
  using QObject::QObject;

public slots:
  void setSeparatorVisible(bool visible)
  {
    _should_replace = !visible;
  }

protected:
  QString process(const QString& fmt, const QString& seps, const QDateTime& dt) override
  {
    Q_UNUSED(dt);
    auto res = fmt;
    using namespace Qt::StringLiterals;
    if (_should_replace) {
      auto pattern = QString("[%1]").arg(QRegularExpression::escape(seps));
      res.replace(QRegularExpression(pattern), u" "_s);
    }
    return res;
  }

private:
  bool _should_replace = false;
};
