#pragma once

#include <QDateTime>
#include <QStringView>

class DateTimeStringBuilder {
public:
  virtual ~DateTimeStringBuilder() = default;

  virtual void addCharacter(char32_t) {}
  virtual void addSeparator(char32_t) {}
};

// in format string only ':' is considered as separator
void FormatDateTime(const QDateTime& dt, QStringView fmt,
                    DateTimeStringBuilder& str_builder);