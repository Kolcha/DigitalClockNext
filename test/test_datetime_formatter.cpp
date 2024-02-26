/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

  void tokenStart(QStringView token) override { _tokens[token.toString()] += 1; }
  void tokenEnd(QStringView token)   override { _tokens[token.toString()] -= 1; }

  QString result() const { return QString::fromUcs4(_result.data(), _result.size()); }
  QString separators() const { return QString::fromUcs4(_seps.data(), _seps.size()); }
  const auto& tokens() const noexcept { return _tokens; }

  void reset()
  {
    _result.clear();
    _seps.clear();
    _tokens.clear();
  }

private:
  QVector<char32_t> _result;
  QVector<char32_t> _seps;
  QHash<QString, int> _tokens;
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
  void testTokenNotify();

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

void DateTimeFormatterTest::testTokenNotify()
{
  // should consider only hh/mm/ss as tokens
  QString fmt = "'foo'h:mm:ss'bar'";
  FormatDateTime(dt, fmt, sb);
  QCOMPARE(sb.tokens().size(), 3);
  QVERIFY(sb.tokens().contains("h"));
  QVERIFY(sb.tokens().contains("mm"));
  QVERIFY(sb.tokens().contains("ss"));
  QCOMPARE(sb.tokens()["h"], 0);
  QCOMPARE(sb.tokens()["mm"], 0);
  QCOMPARE(sb.tokens()["ss"], 0);
}

QTEST_MAIN(DateTimeFormatterTest)

#include "test_datetime_formatter.moc"
