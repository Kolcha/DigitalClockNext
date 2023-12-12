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

#include "linear_layout.hpp"
#include "layout.hpp"
#include "fake_resource.hpp"

class LinearLayoutTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void defaultConstruct();
  void horizontal();
  void vertical();
  void changeSpacing();
  void changeOrientation();

private:
  void addItem(auto&&... args)
  {
    auto res = std::make_shared<FakeResource>(args...);
    _items.push_back(std::make_shared<SimpleGlyph>(res));
  }

private:
  LayoutAlgorithm::ContainerType _items;
};

void LinearLayoutTest::init()
{
  addItem(QRectF( 0, -4, 5, 6), 5, 4);
  addItem(QRectF(-1, -3, 4, 7), 5, 8);
  addItem(QRectF(-1, -5, 4, 6), 3, 4);
  addItem(QRectF(-2, -2, 5, 5), 5, 5);
}

void LinearLayoutTest::cleanup()
{
  _items.clear();
}

void LinearLayoutTest::defaultConstruct()
{
  LinearLayout ll;
  QCOMPARE(ll.orientation(), Qt::Horizontal);
  QCOMPARE(ll.spacing(), 0.0);
  ll.apply(LayoutAlgorithm::ContainerType());
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
  LinearLayout ll(Qt::Horizontal, 0.0);
  QCOMPARE(ll.orientation(), Qt::Horizontal);
  QCOMPARE(ll.spacing(), 0.0);
  ll.apply(_items);
  QCOMPARE(_items.size(), 4);
  QCOMPARE(_items[0]->geometry(), QRectF( 0, -4, 5, 6));
  QCOMPARE(_items[1]->geometry(), QRectF( 4, -3, 4, 7));
  QCOMPARE(_items[2]->geometry(), QRectF( 9, -5, 4, 6));
  QCOMPARE(_items[3]->geometry(), QRectF(11, -2, 5, 5));
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
  LinearLayout ll(Qt::Vertical, 0.0);
  QCOMPARE(ll.orientation(), Qt::Vertical);
  QCOMPARE(ll.spacing(), 0.0);
  ll.apply(_items);
  QCOMPARE(_items.size(), 4);
  QCOMPARE(_items[0]->geometry(), QRectF( 0, -4, 5, 6));
  QCOMPARE(_items[1]->geometry(), QRectF(-1,  1, 4, 7));
  QCOMPARE(_items[2]->geometry(), QRectF(-1,  7, 4, 6));
  QCOMPARE(_items[3]->geometry(), QRectF(-2, 14, 5, 5));
}

void LinearLayoutTest::changeSpacing()
{
  LinearLayout ll(Qt::Horizontal, 0.0);
  QCOMPARE(ll.spacing(), 0.0);
  ll.setSpacing(2.0);
  QCOMPARE(ll.spacing(), 2.0);
  ll.apply(_items);
  QCOMPARE(_items.size(), 4);
  QCOMPARE(_items[0]->geometry(), QRectF( 0, -4, 5, 6));
  QCOMPARE(_items[1]->geometry(), QRectF( 6, -3, 4, 7));
  QCOMPARE(_items[2]->geometry(), QRectF(13, -5, 4, 6));
  QCOMPARE(_items[3]->geometry(), QRectF(17, -2, 5, 5));
}

void LinearLayoutTest::changeOrientation()
{
  LinearLayout ll(Qt::Horizontal, 0.0);
  QCOMPARE(ll.orientation(), Qt::Horizontal);
  ll.setOrientation(Qt::Vertical);
  QCOMPARE(ll.orientation(), Qt::Vertical);
  ll.apply(_items);
  QCOMPARE(_items.size(), 4);
  QCOMPARE(_items[0]->geometry(), QRectF( 0, -4, 5, 6));
  QCOMPARE(_items[1]->geometry(), QRectF(-1,  1, 4, 7));
  QCOMPARE(_items[2]->geometry(), QRectF(-1,  7, 4, 6));
  QCOMPARE(_items[3]->geometry(), QRectF(-2, 14, 5, 5));
}

QTEST_MAIN(LinearLayoutTest)

#include "test_linear_layout.moc"
