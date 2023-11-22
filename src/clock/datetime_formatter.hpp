#pragma once

#include <QDateTime>
#include <QString>

class DateTimeFormatter {
public:
  explicit DateTimeFormatter(QString format)
    : _format(std::move(format))
  {}

  QString process(const QDateTime& dt) const
  {
    // TODO: extended formats support,
    // TODO: e.g. week/day number and etc.
    auto fmt = _format;
    return dt.toString(fmt);
  }

  void setFormat(QString format)
  {
    if (format.isEmpty() || format == _format)
      return;
    _format = std::move(format);
  }

  QString format() const { return _format; }

private:
  QString _format;
};
