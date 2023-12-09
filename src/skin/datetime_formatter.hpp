#pragma once

#include <QDateTime>
#include <QStringView>

class DateTimeStringBuilder {
public:
  virtual ~DateTimeStringBuilder() = default;

  virtual void addCharacter(QChar) {}
  virtual void addSeparator(QChar) {}
};

void FormatDateTime(const QDateTime& dt, QStringView fmt,
                    DateTimeStringBuilder& str_builder);
