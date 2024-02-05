/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTest>

#include "layout.hpp"

static_assert(!std::is_default_constructible_v<LayoutItem>, "is_default_constructible");
static_assert(!std::is_copy_constructible_v<LayoutItem>, "is_copy_constructible");
static_assert(std::is_move_constructible_v<LayoutItem>, "!is_move_constructible");
static_assert(!std::is_copy_assignable_v<LayoutItem>, "is_copy_assignable");
static_assert(std::is_move_assignable_v<LayoutItem>, "!is_move_assignable");

namespace {

class FakeItem final : public LayoutItem
{
public:
  FakeItem() : LayoutItem(std::make_shared<InvisibleResource>(QRectF(0, 0, 5, 5), 5, 5))
  {
  }

  int geometryUpdateCount() const noexcept { return _update_counter; }

protected:
  void doUpdateGeometry() override
  {
    ++_update_counter;
  }

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
  void testResizeItem();

private:
  std::shared_ptr<LayoutItem> _test_item;
  std::shared_ptr<FakeItem> _fake_item;

  static constexpr QRectF r = QRectF(-2.0, -15.0, 10.0, 20.0);
  static constexpr qreal ax = 8.0;
  static constexpr qreal ay = 16.0;
};

void LayoutItemTest::init()
{
  _fake_item = std::make_shared<FakeItem>();
  auto res = std::make_shared<InvisibleResource>(r, ax, ay);
  _test_item = std::make_shared<LayoutItem>(res);
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
  QCOMPARE(_test_item->rect(), _test_item->resource()->rect());
  QCOMPARE(_test_item->pos(), QPointF(0, 0));
  QCOMPARE(_test_item->ax(), ax);
  QCOMPARE(_test_item->ay(), ay);
  QVERIFY(_test_item->parent());
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
}

void LayoutItemTest::applyTransform()
{
  // no change propagation on setTransform()
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  QPointF pos(20, 50);
  _test_item->setPos(pos);
  _test_item->setTransform(QTransform().scale(2.0, 1.5));
  QCOMPARE_NE(_test_item->rect(), _test_item->resource()->rect());
  QCOMPARE_NE(_test_item->rect(), r);
  QCOMPARE_NE(_test_item->ax(), ax);
  QCOMPARE_NE(_test_item->ay(), ay);
  QCOMPARE_EQ(_test_item->resource()->rect(), r);
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
}

void LayoutItemTest::updateGeometry()
{
  // setPos() should not propagate change
  QCOMPARE(_test_item->pos(), QPointF(0, 0));
  QPointF pos(20, 50);
  _test_item->setPos(pos);
  QCOMPARE(_test_item->pos(), pos);
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
  // should propagate geometry change
  QCOMPARE(_test_item->parent().get(), _fake_item.get());
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
  _test_item->updateGeometry();
  QCOMPARE(_fake_item->geometryUpdateCount(), 1);
  // should not crash without parent
  _test_item->setParent(std::shared_ptr<LayoutItem>());
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

void LayoutItemTest::testResizeItem()
{
  // resize is disabled by default
  QVERIFY(!_test_item->resizeEnabled());
  // should be possible to enable
  _test_item->enableResize();
  QVERIFY(_test_item->resizeEnabled());
  // resize in horizontal direction (should keep aspect ratio)
  _test_item->resize(2*r.width(), Qt::Horizontal);
  QCOMPARE(_test_item->rect(), QRectF(2*r.x(), 2*r.y(), 2*r.width(), 2*r.height()));
  QCOMPARE(_test_item->ax(), 2*ax);
  QCOMPARE(_test_item->ay(), 2*ay);
  QCOMPARE(_test_item->resource()->rect(), r);
  // disabling resize should reset geometry
  _test_item->disableResize();
  QVERIFY(!_test_item->resizeEnabled());
  QCOMPARE(_test_item->rect(), r);
  QCOMPARE(_test_item->ax(), ax);
  QCOMPARE(_test_item->ay(), ay);
  // should keep original size after re-enabling resize
  _test_item->enableResize();
  QVERIFY(_test_item->resizeEnabled());
  QCOMPARE(_test_item->rect(), r);
  QCOMPARE(_test_item->ax(), ax);
  QCOMPARE(_test_item->ay(), ay);
  // resize in vertical direction (should keep aspect ratio)
  _test_item->resize(2*r.height(), Qt::Vertical);
  QCOMPARE(_test_item->rect(), QRectF(2*r.x(), 2*r.y(), 2*r.width(), 2*r.height()));
  QCOMPARE(_test_item->ax(), 2*ax);
  QCOMPARE(_test_item->ay(), 2*ay);
  QCOMPARE(_test_item->resource()->rect(), r);
  // should not propagate geometry change
  QCOMPARE(_fake_item->geometryUpdateCount(), 0);
}

QTEST_MAIN(LayoutItemTest)

#include "test_layout_item.moc"
