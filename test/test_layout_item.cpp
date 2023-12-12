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

#include "layout.hpp"
#include "fake_resource.hpp"

static_assert(!std::is_default_constructible_v<SimpleGlyph>, "is_default_constructible");
static_assert(!std::is_copy_constructible_v<SimpleGlyph>, "is_copy_constructible");
static_assert(std::is_move_constructible_v<SimpleGlyph>, "!is_move_constructible");
static_assert(!std::is_copy_assignable_v<SimpleGlyph>, "is_copy_assignable");
static_assert(std::is_move_assignable_v<SimpleGlyph>, "!is_move_assignable");

namespace {

class FakeItem final : public GlyphBase {
public:
  FakeItem() : GlyphBase()
  {
    setGeometry(QRectF(0, 0, 10, 10), 10, 10);
  }

  size_t cacheKey() const override { return 0; }

  int geometryUpdateCount() const noexcept { return _update_counter; }

protected:
  void doUpdateGeometry() override
  {
    ++_update_counter;
  }

  void doDraw([[maybe_unused]] QPainter* p) override {}

private:
  int _update_counter = 0;
};

} // namespace

class LayoutItemTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void initialization();
  void applyTransform();
  void updateGeometry();
  void parentTracking();

private:
  std::shared_ptr<SimpleGlyph> _test_item;
  std::shared_ptr<FakeItem> _fake_item;

  static constexpr QRectF r = QRectF(-2.0, -15.0, 10.0, 20.0);
  static constexpr qreal ax = 8.0;
  static constexpr qreal ay = 16.0;
};

void LayoutItemTest::init()
{
  _fake_item = std::make_shared<FakeItem>();
  auto res = std::make_shared<FakeResource>(r, ax, ay);
  _test_item = std::make_shared<SimpleGlyph>(res);
  _test_item->setParent(_fake_item);
}

void LayoutItemTest::cleanup()
{
  _fake_item.reset();
  _test_item.reset();
}

void LayoutItemTest::initialization()
{
  QCOMPARE(_test_item->rect(), r);
  QCOMPARE(_test_item->transform(), QTransform());
  QCOMPARE(_test_item->geometry(), _test_item->rect());
  QCOMPARE(_test_item->advanceX(), ax);
  QCOMPARE(_test_item->advanceY(), ay);
  QVERIFY(_test_item->parent());
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
}

void LayoutItemTest::applyTransform()
{
  // no change propagation on setTransform()
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  _test_item->setTransform(QTransform().scale(2.0, 1.5));
  QCOMPARE_NE(_test_item->geometry(), _test_item->rect());
  QCOMPARE_NE(_test_item->geometry(), r);
  QCOMPARE_NE(_test_item->advanceX(), ax);
  QCOMPARE_NE(_test_item->advanceY(), ay);
  QCOMPARE_EQ(_test_item->rect(), r);
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
  // no change propagation on resetGeometry()
  _test_item->resetGeometry();
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  QCOMPARE(_test_item->transform(), QTransform());
  QCOMPARE(_test_item->rect(), r);
  QCOMPARE(_test_item->geometry(), _test_item->rect());
  QCOMPARE(_test_item->advanceX(), ax);
  QCOMPARE(_test_item->advanceY(), ay);
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
}

void LayoutItemTest::updateGeometry()
{
  // should propagate geometry change
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
  _test_item->updateGeometry();
  QCOMPARE(_fake_item->geometryUpdateCount(), 1);
  // should not crash without parent
  _test_item->setParent(std::shared_ptr<SimpleGlyph>());
  QVERIFY(!_test_item->parent());
  _test_item->updateGeometry();
  QCOMPARE(_fake_item->geometryUpdateCount(), 1);
}

void LayoutItemTest::parentTracking()
{
  // should keep only reference to parent
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  QCOMPARE(_fake_item.use_count(), 1);
  QCOMPARE(_test_item.use_count(), 1);
  auto parent_item = _test_item->parent();
  QCOMPARE(parent_item.use_count(), _fake_item.use_count());
  QCOMPARE(_fake_item.use_count(), 2);
  QCOMPARE(_test_item.use_count(), 1);
  _fake_item.reset();
  QCOMPARE(parent_item.use_count(), 1);
  QVERIFY(_test_item->parent());
  parent_item.reset();
  QVERIFY(!_test_item->parent());
  QCOMPARE(_test_item.use_count(), 1);
}

QTEST_MAIN(LayoutItemTest)

#include "test_layout_item.moc"
