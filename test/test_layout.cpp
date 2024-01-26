/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTest>

#include <algorithm>
#include <ranges>

#include "layout.hpp"

namespace {

class TestItem : public LayoutItem {
public:
  TestItem(QRectF r, qreal ax, qreal ay)
      : LayoutItem(std::make_shared<InvisibleResource>(std::move(r), ax, ay))
  {}
};


template<typename L>
  requires std::is_base_of_v<LayoutItem, L>
class UpdateCounter final : public L {
public:
  using L::L;

  int geometryUpdateCount() const noexcept
  {
    return _update_counter;
  }

protected:
  void doUpdateGeometry() override
  {
    ++_update_counter;
    L::doUpdateGeometry();
  }

private:
  int _update_counter = 0;
};


class TestLayout : public Layout {
public:
  explicit TestLayout(qreal dx = 2.0) noexcept : _dx(dx) {}

  void setSpacing(qreal dx) noexcept { _dx = dx; }

protected:
  void doAddItem(std::shared_ptr<LayoutItem> item) override
  {
    _items.push_back(std::move(item));
  }

  std::pair<qreal, qreal> doBuildLayout() override
  {
    Q_ASSERT(!_items.empty());
    for (size_t i = 0; i < _items.size(); i++)
      _items[i]->setPos({_dx*i, 0});
    auto geth = [](const auto& i) { return i->rect().height(); };
    return {_dx*_items.size(), std::ranges::max(_items | std::views::transform(geth))};
  }

private:
  std::vector<std::shared_ptr<LayoutItem>> _items;
  qreal _dx;
};

// Layout is a base class and it contains pure virtual methods,
// therefore can't be used in static_assert statements
// use TestLayout instead to verify required properties
static_assert(std::is_default_constructible_v<TestLayout>, "!is_default_constructible");
static_assert(!std::is_copy_constructible_v<TestLayout>, "is_copy_constructible");
static_assert(std::is_move_constructible_v<TestLayout>, "!is_move_constructible");
static_assert(!std::is_copy_assignable_v<TestLayout>, "is_copy_assignable");
static_assert(std::is_move_assignable_v<TestLayout>, "!is_move_assignable");

} // namespace

class LayoutTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void addItems();
  void nestedLayouts();
  void itemsOwnership();
  void assignParent();

private:
  std::shared_ptr<UpdateCounter<TestLayout>> _test_layout;
  std::shared_ptr<UpdateCounter<TestLayout>> _parent_layout;

  static constexpr QRectF r = QRectF(0, 0, 4, 4);
};

void LayoutTest::init()
{
  _parent_layout = std::make_shared<UpdateCounter<TestLayout>>();
  _test_layout = std::make_shared<UpdateCounter<TestLayout>>();
  _parent_layout->addItem(_test_layout);
}

void LayoutTest::cleanup()
{
  _test_layout.reset();
  _parent_layout.reset();
}

void LayoutTest::addItems()
{
  // addItem() should not cause geometry update
  auto new_item = std::make_shared<UpdateCounter<TestItem>>(r, r.width(), r.height());
  _test_layout->addItem(new_item);
  QVERIFY(_test_layout->rect().isNull());
  QCOMPARE(new_item->geometryUpdateCount(), 0);
  QCOMPARE(_test_layout->geometryUpdateCount(), 0);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 0);
  // updateGeometry() propagates the change
  _test_layout->updateGeometry();
  QCOMPARE(_test_layout->rect(), r);
  QCOMPARE(new_item->geometryUpdateCount(), 0);
  QCOMPARE(_test_layout->geometryUpdateCount(), 1);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 1);
}

void LayoutTest::nestedLayouts()
{
  // should propagate geometry changes through nested layouts
  auto build_layout = []() {
    auto l = std::make_shared<TestLayout>(r.width());
    l->addItem(std::make_shared<TestItem>(r, r.width(), r.height()));
    l->addItem(std::make_shared<TestItem>(r, r.width(), r.height()));
    l->addItem(std::make_shared<TestItem>(r, r.width(), r.height()));
    l->updateGeometry();
    return l;
  };

  auto l1 = build_layout();
  QCOMPARE(l1->rect(), QRectF(0, 0, 3*r.width(), r.height()));
  _test_layout->addItem(l1);

  auto l2 = build_layout();
  QCOMPARE(l2->rect(), QRectF(0, 0, 3*r.width(), r.height()));
  _test_layout->addItem(l2);

  auto l3 = build_layout();
  QCOMPARE(l3->rect(), QRectF(0, 0, 3*r.width(), r.height()));
  _test_layout->addItem(l3);

  _test_layout->setSpacing(3*r.width());
  _test_layout->updateGeometry();
  QCOMPARE(_test_layout->rect(), QRectF(0, 0, 9*r.width(), r.height()));
  QCOMPARE(_test_layout->geometryUpdateCount(), 1);

  QCOMPARE(l2->items().size(), 3);
  l2->items()[1]->updateGeometry();
  QCOMPARE(_test_layout->geometryUpdateCount(), 2);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 2);
}

void LayoutTest::itemsOwnership()
{
  // should take ownership of items
  auto item = std::make_shared<TestLayout>();
  _test_layout->addItem(item);
  auto uc = item.use_count();
  QCOMPARE(_test_layout->items().size(), 1);
  item.reset();
  QCOMPARE(_test_layout->items().size(), 1);
  QCOMPARE(_test_layout->items().front().use_count(), uc-1);
  QVERIFY(_test_layout->items().front());
}

void LayoutTest::assignParent()
{
  // should set self as parent for added item
  auto item = std::make_shared<TestLayout>();
  QVERIFY(!item->parent());
  _test_layout->addItem(item);
  QVERIFY(item->parent());
  QCOMPARE(item->parent().get(), _test_layout.get());
}

QTEST_MAIN(LayoutTest)

#include "test_layout.moc"
