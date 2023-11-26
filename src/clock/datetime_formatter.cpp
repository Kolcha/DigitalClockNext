#include "datetime_formatter.hpp"

#include <QLocale>

namespace {

qsizetype repeat_count(qsizetype pos, QStringView str) noexcept
{
  qsizetype count = 0;
  for (auto i = pos; i < str.size() && str[i] == str[pos]; ++i)
    ++count;
  return count;
}

QChar escape_char(QChar c) noexcept
{
  switch (c.unicode()) {
    case 'n':
      return QLatin1Char('\n');
    default:
      return c;
  }
}

void add_characters(QStringView chars, DateTimeStringBuilder& builder)
{
  for (const auto& c : std::as_const(chars)) builder.addCharacter(c);
}

void format_number(int n, int pad, DateTimeStringBuilder& str_builder)
{
  add_characters(QString("%L1").arg(n, pad, 10, QChar('0')), str_builder);
}

} // namespace

void FormatDateTime(const QDateTime& dt, QStringView fmt,
                    DateTimeStringBuilder& str_builder)
{
  bool escaped = false;
  bool quoted = false;

  for (qsizetype i = 0; i < fmt.size(); ++i) {
    const auto& c = fmt[i];

    if (escaped) {
      escaped = false;
      str_builder.addCharacter(escape_char(c));
      continue;
    }

    if (c == '\\') {
      escaped = true;
      continue;
    }

    if (c == '\'') {
      quoted = !quoted;
      continue;
    }

    if (quoted) {
      str_builder.addCharacter(c);
      continue;
    }

    int repeat = repeat_count(i, fmt);

    switch (c.unicode()) {
      case 'h': {
        repeat = qMin(repeat, 2);
        auto h = dt.time().hour();
        format_number(h > 12 ? h - 12 : h, repeat == 2 ? 2 : 0, str_builder);
        break;
      }
      case 'J':
        repeat = qMin(repeat, 1);
        format_number(dt.date().dayOfYear(), 0, str_builder);
        break;
      case 'W':
        repeat = qMin(repeat, 2);
        format_number(dt.date().weekNumber(), repeat == 2 ? 2 : 0, str_builder);
        break;
      case ':':
        str_builder.addSeparator(c);
        break;
      case 'a':
      case 'A':
        repeat = qMin(repeat, 1);
        if (i + 1 < fmt.size() && (fmt[i+1] == 'p' || fmt[i+1] == 'P'))
          repeat += 1;  // AP should be handled as 'A' (case insensitive)
        [[fallthrough]];
      default:
        add_characters(QLocale::system().toString(dt, fmt.mid(i, repeat)), str_builder);
    }

    i += repeat - 1;
  }
}
