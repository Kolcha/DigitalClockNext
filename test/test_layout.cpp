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

static_assert(std::is_default_constructible_v<CompositeGlyph>, "!is_default_constructible");
static_assert(!std::is_copy_constructible_v<CompositeGlyph>, "is_copy_constructible");
static_assert(std::is_move_constructible_v<CompositeGlyph>, "!is_move_constructible");
static_assert(!std::is_copy_assignable_v<CompositeGlyph>, "is_copy_assignable");
static_assert(std::is_move_assignable_v<CompositeGlyph>, "!is_move_assignable");

namespace {

class TestGlyph : public SimpleGlyph {
public:
  TestGlyph(QRectF r, qreal ax, qreal ay)
      : SimpleGlyph(std::make_shared<FakeResource>(std::move(r), ax, ay))
  {}
};


template<typename L>
  requires std::is_base_of_v<GlyphBase, L>
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


class TestAlgorithm final : public LayoutAlgorithm {
public:
  explicit TestAlgorithm(qreal dx) noexcept : _dx(dx) {}

private:
  void apply(const ContainerType& items) const override
  {
    for (size_t i = 0; i < items.size(); i++)
      items[i]->setPos({_dx*i, 0});
  }

private:
  qreal _dx;
};

} // namespace

class LayoutTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void addGlyphs();
  void algorithmChange();
  void nestedLayouts();
  void algorithmOwnership();
  void itemsOwnership();
  void assignParent();

private:
  std::shared_ptr<UpdateCounter<CompositeGlyph>> _test_layout;
  std::shared_ptr<UpdateCounter<CompositeGlyph>> _parent_layout;

  static constexpr QRectF r = QRectF(0, 0, 4, 4);
};

void LayoutTest::init()
{
  _parent_layout = std::make_shared<UpdateCounter<CompositeGlyph>>();
  _test_layout = std::make_shared<UpdateCounter<CompositeGlyph>>();
  _test_layout->setParent(_parent_layout);
}

void LayoutTest::cleanup()
{
  _test_layout.reset();
  _parent_layout.reset();
}

void LayoutTest::addGlyphs()
{
  // addGlyph() should not cause geometry update
  auto new_item = std::make_shared<UpdateCounter<TestGlyph>>(r, r.width(), r.height());
  _test_layout->addGlyph(new_item);
  QVERIFY(_test_layout->rect().isNull());
  QVERIFY(_test_layout->geometry().isNull());
  QCOMPARE(new_item->geometryUpdateCount(), 0);
  QCOMPARE(_test_layout->geometryUpdateCount(), 0);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 0);
  // updateGeometry() propagates the change
  _test_layout->updateGeometry();
  QCOMPARE(_test_layout->rect(), r);
  QCOMPARE(_test_layout->geometry(), _test_layout->rect());
  QCOMPARE(new_item->geometryUpdateCount(), 0);
  QCOMPARE(_test_layout->geometryUpdateCount(), 1);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 1);
}

void LayoutTest::algorithmChange()
{
  // setAlgorithm() should not cause geometry update
  _test_layout->addGlyph(std::make_shared<TestGlyph>(r, r.width(), r.height()));
  _test_layout->addGlyph(std::make_shared<TestGlyph>(r, r.width(), r.height()));
  _test_layout->updateGeometry();
  QCOMPARE(_test_layout->rect(), r);
  QCOMPARE(_test_layout->geometryUpdateCount(), 1);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 1);
  _test_layout->setAlgorithm(std::make_shared<TestAlgorithm>(r.width()));
  QCOMPARE(_test_layout->rect(), r);
  QCOMPARE(_test_layout->geometryUpdateCount(), 1);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 1);
  // changes are applied only on updateGeometry()
  _test_layout->updateGeometry();
  QCOMPARE(_test_layout->rect(), QRectF(0, 0, 2*r.width(), r.height()));
  QCOMPARE(_test_layout->geometryUpdateCount(), 2);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 2);
}

void LayoutTest::nestedLayouts()
{
  // should propagate geometry changes through nested layouts
  auto build_layout = [](auto algo) {
    auto l = std::make_shared<CompositeGlyph>(std::move(algo));
    l->addGlyph(std::make_shared<TestGlyph>(r, r.width(), r.height()));
    l->addGlyph(std::make_shared<TestGlyph>(r, r.width(), r.height()));
    l->addGlyph(std::make_shared<TestGlyph>(r, r.width(), r.height()));
    l->updateGeometry();
    return l;
  };

  auto algo = std::make_shared<TestAlgorithm>(r.width());

  auto l1 = build_layout(algo);
  QCOMPARE(l1->rect(), QRectF(0, 0, 3*r.width(), r.height()));
  _test_layout->addGlyph(l1);

  auto l2 = build_layout(algo);
  QCOMPARE(l2->rect(), QRectF(0, 0, 3*r.width(), r.height()));
  _test_layout->addGlyph(l2);

  auto l3 = build_layout(algo);
  QCOMPARE(l3->rect(), QRectF(0, 0, 3*r.width(), r.height()));
  _test_layout->addGlyph(l3);

  _test_layout->setAlgorithm(std::make_unique<TestAlgorithm>(3*r.width()));
  _test_layout->updateGeometry();
  QCOMPARE(_test_layout->rect(), QRectF(0, 0, 9*r.width(), r.height()));
  QCOMPARE(_test_layout->geometryUpdateCount(), 1);

  QCOMPARE(l2->items().size(), 3);
  l2->items()[1]->updateGeometry();
  QCOMPARE(_test_layout->geometryUpdateCount(), 2);
  QCOMPARE(_parent_layout->geometryUpdateCount(), 2);
}

void LayoutTest::algorithmOwnership()
{
  // should take ownership of algorithm object
  auto algo = std::make_shared<TestAlgorithm>(25);
  _test_layout->setAlgorithm(algo);
  QVERIFY(_test_layout->algorithm());
  QCOMPARE(algo.use_count(), 2);
  algo.reset();
  QVERIFY(_test_layout->algorithm());
}

void LayoutTest::itemsOwnership()
{
  // should take ownership of items
  auto item = std::make_shared<CompositeGlyph>();
  _test_layout->addGlyph(item);
  QCOMPARE(item.use_count(), 2);
  QCOMPARE(_test_layout->items().size(), 1);
  item.reset();
  QCOMPARE(_test_layout->items().size(), 1);
  QCOMPARE(_test_layout->items().front().use_count(), 1);
  QVERIFY(_test_layout->items().front());
}

void LayoutTest::assignParent()
{
  // should set self as parent for added item
  auto item = std::make_shared<CompositeGlyph>();
  QVERIFY(!item->parent());
  _test_layout->addGlyph(item);
  QVERIFY(item->parent());
  QCOMPARE(item->parent().get(), _test_layout.get());
}

QTEST_MAIN(LayoutTest)

#include "test_layout.moc"
