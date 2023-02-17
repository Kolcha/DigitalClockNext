#pragma once

#include <QDateTime>
#include <QString>

class DateTimeFormatter {
public:
  explicit DateTimeFormatter(QString format)
    : _format(std::move(format))
    , _separators(".:/|*-")
  {}

  QString process(const QDateTime& dt) const
  {
    // TODO: extended formats support,
    // TODO: e.g. week/day number and etc.
    return dt.toString(_format);
  }

  void setFormat(QString format)
  {
    if (format.isEmpty() || format == _format)
      return;
    _format = std::move(format);
  }

  QString format() const { return _format; }

  bool isSeparator(QChar ch) const
  {
    return _separators.contains(ch);
  }

  void setSeparators(QString seps)
  {
    if (seps.isEmpty() || seps == _separators)
      return;
    _separators = std::move(seps);
  }

private:
  QString _format;
  QString _separators;
};
