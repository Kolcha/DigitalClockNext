#pragma once

#include <QObject>
#include <QRegularExpression>
#include "datetime_formatter.hpp"

class LegacySkinExtension final : public QObject,
                                  public DateTimeFormatter::Extension
{
  Q_OBJECT

public:
  using QObject::QObject;

  QString name() const override { return QLatin1String("legacy_skin"); }

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
