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

#include "core/item_geometry.hpp"

static_assert(std::is_default_constructible_v<ItemGeometry>, "!is_default_constructible");
static_assert(std::is_copy_constructible_v<ItemGeometry>, "!is_copy_constructible");
static_assert(std::is_move_constructible_v<ItemGeometry>, "!is_move_constructible");
static_assert(std::is_copy_assignable_v<ItemGeometry>, "!is_copy_assignable");
static_assert(std::is_move_assignable_v<ItemGeometry>, "!is_move_assignable");

class ItemGeometryTest : public QObject
{
  Q_OBJECT

private slots:
  void defaultConstruct();
  void constructFromRectAndAxAy();
  void constructFromAllParameters();
  void constructFromRectParameters();
  void constructFromRect();
  void constructFromSize();
  void setRect();
  void setAdvance();
  void comparison();

private:
  // test data to create geometry
  static constexpr QRectF r = QRectF(-2.0, -15.0, 10.0, 20.0);
  static constexpr qreal x = ItemGeometryTest::r.x();
  static constexpr qreal y = ItemGeometryTest::r.y();
  static constexpr qreal w = ItemGeometryTest::r.width();
  static constexpr qreal h = ItemGeometryTest::r.height();
  static constexpr qreal ax = 8.0;
  static constexpr qreal ay = 12.0;
};

void ItemGeometryTest::defaultConstruct()
{
  constexpr ItemGeometry g;
  QVERIFY(g.rect().isNull());
  QCOMPARE(g.rect(), QRectF());
}

void ItemGeometryTest::constructFromRectAndAxAy()
{
  constexpr ItemGeometry g(r, ax, ay);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), ax);
  QCOMPARE(g.advanceY(), ay);
}

void ItemGeometryTest::constructFromAllParameters()
{
  constexpr ItemGeometry g(x, y, w, h, ax, ay);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), ax);
  QCOMPARE(g.advanceY(), ay);
}

void ItemGeometryTest::constructFromRectParameters()
{
  constexpr ItemGeometry g(x, y, w, h);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), w);
  QCOMPARE(g.advanceY(), h);
}

void ItemGeometryTest::constructFromRect()
{
  constexpr ItemGeometry g(r);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), r.width());
  QCOMPARE(g.advanceY(), r.height());
}

void ItemGeometryTest::constructFromSize()
{
  constexpr ItemGeometry g(QSizeF(w, h));
  QCOMPARE(g.rect(), QRectF(0, 0, w, h));
  QCOMPARE(g.advanceX(), w);
  QCOMPARE(g.advanceY(), h);
}

void ItemGeometryTest::setRect()
{
  ItemGeometry g(r, ax, ay);

  QRectF new_rect(-5, -18, 12, 24);
  const QRectF c_new_rect = new_rect;
  g.setRect(std::move(new_rect));
  QCOMPARE(g.rect(), c_new_rect);
  QCOMPARE(g.advanceX(), ax);
  QCOMPARE(g.advanceY(), ay);

  g.setRect(r);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), ax);
  QCOMPARE(g.advanceY(), ay);
}

void ItemGeometryTest::setAdvance()
{
  ItemGeometry g(r, ax, ay);

  constexpr qreal new_ax = 12.0;
  constexpr qreal new_ay = 24.0;

  g.setAdvanceX(new_ax);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), new_ax);
  QCOMPARE(g.advanceY(), ay);

  g.setAdvanceY(new_ay);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), new_ax);
  QCOMPARE(g.advanceY(), new_ay);

  g.setAdvance(ax, ay);
  QCOMPARE(g.rect(), r);
  QCOMPARE(g.advanceX(), ax);
  QCOMPARE(g.advanceY(), ay);
}

void ItemGeometryTest::comparison()
{
  ItemGeometry g1(-2, -11, 12, 18, 10, 16);
  ItemGeometry g2(-3, -10, 10, 12, 11, 13);

  QCOMPARE_NE(g1, ItemGeometry());
  QCOMPARE_NE(g2, ItemGeometry());
  QCOMPARE_NE(g1, g2);

  g1 = g2;
  QCOMPARE_EQ(g1, g2);
  QCOMPARE_EQ(ItemGeometry(), ItemGeometry());
}

QTEST_MAIN(ItemGeometryTest)

#include "test_item_geometry.moc"
