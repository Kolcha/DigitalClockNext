/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTest>

#include "linear_layout.hpp"

static_assert(!std::is_copy_constructible_v<LinearLayout>, "is_copy_constructible");
static_assert(std::is_move_constructible_v<LinearLayout>, "!is_move_constructible");
static_assert(!std::is_copy_assignable_v<LinearLayout>, "is_copy_assignable");
static_assert(std::is_move_assignable_v<LinearLayout>, "!is_move_assignable");

class LinearLayoutTest : public QObject
{
  Q_OBJECT

private slots:
  void defaultConstruct();
  void horizontal();
  void vertical();
  void changeSpacing();
  void changeOrientation();
  void ignoreAdvanceX();
  void ignoreAdvanceY();
  // TODO: test scaling
  // TODO: test alignment
  // TODO: scaling + alignment
  // TODO: test geomtery change propagation (implicitly done?)

private:
  auto createItem(auto&&... args)
  {
    auto res = std::make_shared<InvisibleResource>(args...);
    return std::make_shared<LayoutItem>(std::move(res));
  }

  void addItems(LinearLayout& l)
  {
    l.addItem(createItem(QRectF( 0, -4, 5, 6), 5, 5));
    l.addItem(createItem(QRectF(-1, -3, 4, 7), 5, 4));
    l.addItem(createItem(QRectF(-1, -5, 4, 6), 3, 8));
    l.addItem(createItem(QRectF(-2, -2, 5, 5), 5, 4));
    l.updateGeometry();
  }
};

void LinearLayoutTest::defaultConstruct()
{
  auto ll = std::make_shared<LinearLayout>();
  QCOMPARE(ll->orientation(), Qt::Horizontal);
  QCOMPARE(ll->spacing(), 0.0);
  QVERIFY(!ll->ignoreAdvance());
  QCOMPARE(ll->items().size(), 0);
}

/*
 *  - 0 +
 *    |               +------+
 *    +-------+       |      |
 *    |     +-+----+  |      |
 * -  |     | |    |  |   +--+----+
 * 0 _|_ _ _|_|_ _ |_ |_ _|_ |_ _ |_ _
 * +  |     | |    |  +---+--+    |
 *    +-----+-+    |      |       |
 *          |      |      +-------+
 *          +------+
 */
void LinearLayoutTest::horizontal()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Horizontal);
  QCOMPARE(ll->orientation(), Qt::Horizontal);
  QCOMPARE(ll->spacing(), 0.0);
  addItems(*ll);
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->rect(), QRectF(0, -5, 16, 9));
  // should respect ax()
  QCOMPARE(ll->items()[0]->pos(), QPointF( 0, 0));
  QCOMPARE(ll->items()[1]->pos(), QPointF( 5, 0));
  QCOMPARE(ll->items()[2]->pos(), QPointF(10, 0));
  QCOMPARE(ll->items()[3]->pos(), QPointF(13, 0));
  QCOMPARE(ll->ax(), 18);
  QCOMPARE(ll->ay(), 8);
  // rect remains unchanged
  QCOMPARE(ll->items()[0]->rect(), QRectF( 0, -4, 5, 6));
  QCOMPARE(ll->items()[1]->rect(), QRectF(-1, -3, 4, 7));
  QCOMPARE(ll->items()[2]->rect(), QRectF(-1, -5, 4, 6));
  QCOMPARE(ll->items()[3]->rect(), QRectF(-2, -2, 5, 5));
}

/*
 *      - 0 +
 *        |
 *        +-------+
 *        |       |
 * -      |       |
 * 0 _ _ _|_ _ _ _|_ _
 * +    +-+----+  |
 *      | +----+--+
 *      |      |
 *      | |    |
 *      +------+ - - -
 *      | |    |     |
 *      +------+     |
 *      |      |     |
 *      | |    |     |
 *      +------+ - - -
 *        |
 *    +--------+
 *    |   |    |
 *    |        |
 *    |   |    |
 *    +--------+
 *        |
 */
void LinearLayoutTest::vertical()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical);
  QCOMPARE(ll->orientation(), Qt::Vertical);
  QCOMPARE(ll->spacing(), 0.0);
  addItems(*ll);
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->rect(), QRectF(-2, -4, 7, 23));
  // should respect ay()
  QCOMPARE(ll->items()[0]->pos(), QPointF(0,  0));
  QCOMPARE(ll->items()[1]->pos(), QPointF(0,  4));
  QCOMPARE(ll->items()[2]->pos(), QPointF(0, 12));
  QCOMPARE(ll->items()[3]->pos(), QPointF(0, 16));
  QCOMPARE(ll->ax(), 5);
  QCOMPARE(ll->ay(), 21);
  // rect remains unchanged
  QCOMPARE(ll->items()[0]->rect(), QRectF( 0, -4, 5, 6));
  QCOMPARE(ll->items()[1]->rect(), QRectF(-1, -3, 4, 7));
  QCOMPARE(ll->items()[2]->rect(), QRectF(-1, -5, 4, 6));
  QCOMPARE(ll->items()[3]->rect(), QRectF(-2, -2, 5, 5));
}

void LinearLayoutTest::changeSpacing()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Horizontal);
  addItems(*ll);
  QCOMPARE(ll->spacing(), 0.0);
  ll->setSpacing(2.0);
  QCOMPARE(ll->spacing(), 2.0);
  ll->updateGeometry();
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->rect(), QRectF(0, -5, 22, 9));
  QCOMPARE(ll->items()[0]->pos(), QPointF( 0, 0));
  QCOMPARE(ll->items()[1]->pos(), QPointF( 7, 0));
  QCOMPARE(ll->items()[2]->pos(), QPointF(14, 0));
  QCOMPARE(ll->items()[3]->pos(), QPointF(19, 0));
  QCOMPARE(ll->ax(), 24);
  QCOMPARE(ll->ay(), 8);
}

void LinearLayoutTest::changeOrientation()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Horizontal);
  addItems(*ll);
  QCOMPARE(ll->orientation(), Qt::Horizontal);
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->rect(), QRectF(0, -5, 16, 9));
  QCOMPARE(ll->items()[0]->pos(), QPointF( 0, 0));
  QCOMPARE(ll->items()[1]->pos(), QPointF( 5, 0));
  QCOMPARE(ll->items()[2]->pos(), QPointF(10, 0));
  QCOMPARE(ll->items()[3]->pos(), QPointF(13, 0));
  ll->setOrientation(Qt::Vertical);
  QCOMPARE(ll->orientation(), Qt::Vertical);
  ll->updateGeometry();
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->rect(), QRectF(-2, -4, 7, 23));
  QCOMPARE(ll->items()[0]->pos(), QPointF(0,  0));
  QCOMPARE(ll->items()[1]->pos(), QPointF(0,  4));
  QCOMPARE(ll->items()[2]->pos(), QPointF(0, 12));
  QCOMPARE(ll->items()[3]->pos(), QPointF(0, 16));
}

void LinearLayoutTest::ignoreAdvanceX()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Horizontal, 2.0);
  ll->setIgnoreAdvance(true);
  QCOMPARE(ll->orientation(), Qt::Horizontal);
  QCOMPARE(ll->spacing(), 2.0);
  QVERIFY(ll->ignoreAdvance());
  addItems(*ll);
  QCOMPARE(ll->rect(), QRectF(0, -5, 24, 9));
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->items()[0]->pos(), QPointF( 0, 0));
  QCOMPARE(ll->items()[1]->pos(), QPointF( 8, 0));
  QCOMPARE(ll->items()[2]->pos(), QPointF(14, 0));
  QCOMPARE(ll->items()[3]->pos(), QPointF(21, 0));
  QCOMPARE(ll->ax(), 26);
  QCOMPARE(ll->ay(), 8);
}

void LinearLayoutTest::ignoreAdvanceY()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical, 2.0);
  ll->setIgnoreAdvance(true);
  QCOMPARE(ll->orientation(), Qt::Vertical);
  QCOMPARE(ll->spacing(), 2.0);
  QVERIFY(ll->ignoreAdvance());
  addItems(*ll);
  QCOMPARE(ll->rect(), QRectF(-2, -4, 7, 30));
  QCOMPARE(ll->items().size(), 4);
  QCOMPARE(ll->items()[0]->pos(), QPointF(0,  0));
  QCOMPARE(ll->items()[1]->pos(), QPointF(0,  7));
  QCOMPARE(ll->items()[2]->pos(), QPointF(0, 18));
  QCOMPARE(ll->items()[3]->pos(), QPointF(0, 23));
  QCOMPARE(ll->ax(), 5);
  QCOMPARE(ll->ay(), 28);
}

QTEST_MAIN(LinearLayoutTest)

#include "test_linear_layout.moc"
