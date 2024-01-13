/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

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

#include <QTest>

#include "datetime_formatter.hpp"

namespace {

class SimpleDateTimeStringBuilder final : public DateTimeStringBuilder
{
public:
  void addCharacter(char32_t ch) override { _result.append(ch); }
  void addSeparator(char32_t ch) override
  {
    addCharacter(ch);
    _seps.append(ch);
  }

  QString result() const { return QString::fromUcs4(_result.data(), _result.size()); }
  QString separators() const { return QString::fromUcs4(_seps.data(), _seps.size()); }

  void reset()
  {
    _result.clear();
    _seps.clear();
  }

private:
  QVector<char32_t> _result;
  QVector<char32_t> _seps;
};

} // namespace

class DateTimeFormatterTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void test12hFormat();
  void testWeekNumber();
  void testDayNumber();
  void testEscaping();
  void testQuoting();
  void testSeparator();
  void testComplexCase();
  void testUnicode();

private:
  SimpleDateTimeStringBuilder sb;
  QDateTime dt;
};

void DateTimeFormatterTest::init()
{
  dt = QDateTime(QDate(2023, 12, 19), QTime(0, 30, 56));
}

void DateTimeFormatterTest::cleanup()
{
  sb.reset();
}

void DateTimeFormatterTest::test12hFormat()
{
  // h or hh should produce 12h time regardless of AM/PM
  // no leading zero
  dt.setTime(QTime(8, 20));
  FormatDateTime(dt, u"h", sb);
  QCOMPARE(sb.result(), u"8");
  sb.reset();
  // with leading zero
  FormatDateTime(dt, u"hh", sb);
  QCOMPARE(sb.result(), u"08");
  sb.reset();
  // 12 PM
  dt.setTime(QTime(12, 0));
  FormatDateTime(dt, u"h", sb);
  QCOMPARE(sb.result(), u"12");
  sb.reset();
  // 1 PM
  dt.setTime(QTime(13, 0));
  FormatDateTime(dt, u"hh", sb);
  QCOMPARE(sb.result(), u"01");
  sb.reset();
  // 12 AM
  dt.setTime(QTime(0, 0));
  FormatDateTime(dt, u"hh", sb);
  QCOMPARE(sb.result(), u"12");
  sb.reset();
  // 1 AM
  dt.setTime(QTime(1, 0));
  FormatDateTime(dt, u"hh", sb);
  QCOMPARE(sb.result(), u"01");
}

void DateTimeFormatterTest::testWeekNumber()
{
  // W or WW should produce (zero-padded) week number
  // no leading zero
  dt.setDate(QDate(2023, 2, 14));
  FormatDateTime(dt, u"W", sb);
  QCOMPARE(sb.result(), u"7");
  sb.reset();
  // with leading zero
  FormatDateTime(dt, u"WW", sb);
  QCOMPARE(sb.result(), u"07");
  sb.reset();
  // lowercase 'w' is not interpreted
  FormatDateTime(dt, u"w", sb);
  QCOMPARE(sb.result(), u"w");
}

void DateTimeFormatterTest::testDayNumber()
{
  // J should produce "day of the year"
  // no leading zero
  dt.setDate(QDate(2023, 2, 14));
  FormatDateTime(dt, u"J", sb);
  QCOMPARE(sb.result(), u"45");
  sb.reset();
  // lowercase 'j' is not interpreted
  FormatDateTime(dt, u"j", sb);
  QCOMPARE(sb.result(), u"j");
}

void DateTimeFormatterTest::testEscaping()
{
  // escaped characters should not be parsed
  FormatDateTime(dt, u"\\h\\h\\:\\m\\m", sb);
  QCOMPARE(sb.result(), u"hh:mm");
}

void DateTimeFormatterTest::testQuoting()
{
  // single-quoted characters should not be parsed
  FormatDateTime(dt, u"'hh:mm'", sb);
  QCOMPARE(sb.result(), u"hh:mm");
  sb.reset();
  // escaped characters should be replaced (unescaped)
  FormatDateTime(dt, u"'hh\\nm\\m'", sb);
  QCOMPARE(sb.result(), u"hh\nmm");
}

void DateTimeFormatterTest::testSeparator()
{
  // only ':' should be treated as separator
  FormatDateTime(dt, u"hh:mm.ss d MM", sb);
  QCOMPARE(sb.separators(), u":");
  sb.reset();
  // escaped ':' is not treated as separator
  FormatDateTime(dt, u"hh\\:mm", sb);
  QVERIFY(sb.separators().isEmpty());
  sb.reset();
  // quoted ':' is not treated as separator
  FormatDateTime(dt, u"hh':'mm", sb);
  QVERIFY(sb.separators().isEmpty());
}

void DateTimeFormatterTest::testComplexCase()
{
  QString fmt = "hh:mm:x'\\n'x:ss:W,J,yyyy\\nx";
  FormatDateTime(dt, fmt, sb);
  QCOMPARE(sb.result(), u"12:30:x\nx:56:51,353,2023\nx");
}

void DateTimeFormatterTest::testUnicode()
{
  // should support any Unicode characters
  QString fmt = "\U0001F605hh\U0001F643\U0001F643mm\U0001FAE0";
  FormatDateTime(dt, fmt, sb);
  QCOMPARE(sb.result(), "\U0001F60512\U0001F643\U0001F64330\U0001FAE0");
}

QTEST_MAIN(DateTimeFormatterTest)

#include "test_datetime_formatter.moc"
