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

namespace {

class ParentLayout : public Layout
{
public:
  int updateGeometryCount() const noexcept { return _update_counter; }

protected:
  void doUpdateGeometry() override
  {
    ++_update_counter;
    Layout::doUpdateGeometry();
  }

  // this test layout does nothing, just counts updateGeometry() calls
  void doAddItem(std::shared_ptr<LayoutItem>) noexcept override {}
  std::pair<qreal, qreal> doBuildLayout() noexcept override { return {0, 0}; }

private:
  int _update_counter = 0;
};

inline QRectF g(const LayoutItem& item) noexcept
{
  return item.rect().translated(item.pos());
}

} // namespace

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
  void testScaling1();
  void testScaling2();
  void testScaling3();
  void testAlignmentH();
  void testAlignmentV();
  void testScaleAndAlign();

private:
  auto createItem(auto&& ... args)
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

  // for scaling and alignment tests
  void addItems2(LinearLayout& l)
  {
    l.addItem(createItem(QRectF(-1, -5, 15, 6), 18, 9));
    l.addItem(createItem(QRectF(-1, -5, 10, 6), 10, 9));
    l.addItem(createItem(QRectF(-1, -7, 20, 8), 18, 9));
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

void LinearLayoutTest::testScaling1()
{
  // only one central item is scalable
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical);
  addItems2(*ll);
  QCOMPARE(ll->items().size(), 3);
  QVERIFY(ll->items()[1]->transform().isIdentity());
  ll->items()[1]->enableResize();
  ll->updateGeometry();
  QVERIFY(ll->items()[1]->transform().isScaling());
  QCOMPARE(g(*ll->items()[1]).left(), ll->rect().left());
  QCOMPARE(g(*ll->items()[1]).right(), ll->rect().right());
  QCOMPARE(ll->items()[1]->ax(), 20);
  QCOMPARE(ll->items()[1]->ay(), 18);
}

void LinearLayoutTest::testScaling2()
{
  // two first items are scalable
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical);
  addItems2(*ll);
  QCOMPARE(ll->items().size(), 3);
  QVERIFY(ll->items()[0]->transform().isIdentity());
  QVERIFY(ll->items()[1]->transform().isIdentity());
  ll->items()[0]->enableResize();
  ll->items()[1]->enableResize();
  ll->updateGeometry();
  QVERIFY(ll->items()[0]->transform().isScaling());
  QVERIFY(ll->items()[1]->transform().isScaling());
  QCOMPARE(ll->items()[0]->ax(), 24);
  QCOMPARE(ll->items()[0]->ay(), 12);
  QCOMPARE(ll->items()[1]->ax(), 20);
  QCOMPARE(ll->items()[1]->ay(), 18);
  QCOMPARE(g(*ll->items()[0]).left(), ll->rect().left());
  QCOMPARE(g(*ll->items()[0]).right(), ll->rect().right());
  QCOMPARE(g(*ll->items()[1]).left(), ll->rect().left());
  QCOMPARE(g(*ll->items()[1]).right(), ll->rect().right());
}

void LinearLayoutTest::testScaling3()
{
  // all items are scalable
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical);
  addItems2(*ll);
  QCOMPARE(ll->items().size(), 3);
  QVERIFY(ll->items()[0]->transform().isIdentity());
  QVERIFY(ll->items()[1]->transform().isIdentity());
  QVERIFY(ll->items()[2]->transform().isIdentity());
  ll->items()[0]->enableResize();
  ll->items()[1]->enableResize();
  ll->items()[2]->enableResize();
  ll->updateGeometry();
  // the first item is the reference one
  QVERIFY(ll->items()[0]->transform().isIdentity());
  QVERIFY(ll->items()[1]->transform().isScaling());
  QVERIFY(ll->items()[2]->transform().isScaling());
  QCOMPARE(ll->items()[0]->ax(), 18);
  QCOMPARE(ll->items()[0]->ay(), 9);
  QCOMPARE(ll->items()[1]->ax(), 15);
  QCOMPARE(ll->items()[1]->ay(), 13.5);
  QCOMPARE(ll->items()[2]->ax(), 13.5);
  QCOMPARE(ll->items()[2]->ay(), 6.75);
  QCOMPARE(g(*ll->items()[0]).left(), ll->rect().left());
  QCOMPARE(g(*ll->items()[0]).right(), ll->rect().right());
  QCOMPARE(g(*ll->items()[1]).left(), ll->rect().left());
  QCOMPARE(g(*ll->items()[1]).right(), ll->rect().right());
  QCOMPARE(g(*ll->items()[2]).left(), ll->rect().left());
  QCOMPARE(g(*ll->items()[2]).right(), ll->rect().right());
}

void LinearLayoutTest::testAlignmentH()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical);
  addItems2(*ll);
  QCOMPARE(ll->items().size(), 3);
  QCOMPARE(ll->items()[0]->pos().x(), 0);
  QCOMPARE(ll->items()[1]->pos().x(), 0);
  QCOMPARE(ll->items()[2]->pos().x(), 0);
  ll->setItemAlignment(0, Qt::AlignRight | Qt::AlignBaseline);
  ll->setItemAlignment(1, Qt::AlignHCenter | Qt::AlignBaseline);
  ll->setItemAlignment(2, Qt::AlignLeft | Qt::AlignBaseline);
  ll->updateGeometry();
  QCOMPARE(ll->items()[0]->pos().x(), 5);
  QCOMPARE(ll->items()[1]->pos().x(), 5);
  QCOMPARE(ll->items()[2]->pos().x(), 0);
}

void LinearLayoutTest::testAlignmentV()
{
  auto ll = std::make_shared<LinearLayout>(Qt::Horizontal);
  addItems2(*ll);
  QCOMPARE(ll->items().size(), 3);
  QCOMPARE(ll->items()[0]->pos().y(), 0);
  QCOMPARE(ll->items()[1]->pos().y(), 0);
  QCOMPARE(ll->items()[2]->pos().y(), 0);
  ll->setItemAlignment(0, Qt::AlignTop | Qt::AlignJustify);
  ll->setItemAlignment(1, Qt::AlignVCenter | Qt::AlignJustify);
  ll->setItemAlignment(2, Qt::AlignBottom | Qt::AlignJustify);
  ll->updateGeometry();
  QCOMPARE(ll->items()[0]->pos().y(), -2);
  QCOMPARE(ll->items()[1]->pos().y(), -1);
  QCOMPARE(ll->items()[2]->pos().y(), 0);
}

void LinearLayoutTest::testScaleAndAlign()
{
  // the first item is scaled, the second is aligned
  auto ll = std::make_shared<LinearLayout>(Qt::Vertical);
  addItems2(*ll);
  auto pp = std::make_shared<ParentLayout>();
  pp->addItem(ll);
  QCOMPARE(pp->updateGeometryCount(), 0);
  QCOMPARE(ll->items().size(), 3);
  QVERIFY(ll->items()[0]->transform().isIdentity());
  QCOMPARE(ll->items()[1]->pos().x(), 0);
  ll->items()[0]->enableResize();
  ll->setItemAlignment(1, Qt::AlignRight | Qt::AlignBaseline);
  ll->updateGeometry();
  QCOMPARE(pp->updateGeometryCount(), 1);
  QVERIFY(ll->items()[0]->transform().isScaling());
  QCOMPARE(ll->items()[0]->ax(), 24);
  QCOMPARE(ll->items()[0]->ay(), 12);
  QCOMPARE(ll->items()[1]->pos().x(), 10);
  QVERIFY(ll->items()[2]->transform().isIdentity());
  QCOMPARE(ll->items()[2]->pos().x(), 0);
}

QTEST_MAIN(LinearLayoutTest)

#include "test_linear_layout.moc"
