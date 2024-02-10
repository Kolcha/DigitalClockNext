/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QTest>

#include "layout.hpp"

namespace {

template<typename L>
requires std::is_base_of_v<LayoutItem, L>
class UpdateCounter final : public L
{
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

} // namespace

class PlaceholderItemTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();

  void geometryUpdateWithNoContent();
  void shouldNotUseContentsGeometry();
  void shouldScaleItemThatDoesntFit();
  void shouldApplyContentAlignment();
  void shouldAssignItselfAsParent();
  void noImplicitGeometryUpdates();
  void shouldHandleContentGeometryUpdates();

private:
  std::shared_ptr<UpdateCounter<LayoutItem>> _parent;
  std::shared_ptr<UpdateCounter<PlaceholderItem>> _placeholder;
  std::shared_ptr<UpdateCounter<LayoutItem>> _content;

  static constexpr QRectF r = QRectF(-20, -20, 40, 40);
  static constexpr qreal ax = 36;
  static constexpr qreal ay = 42;
};

void PlaceholderItemTest::init()
{
  auto cr = QRectF(-5, 5, 10, 10);
  auto res = std::make_shared<InvisibleResource>(cr, 5, 5);
  _content = std::make_shared<UpdateCounter<LayoutItem>>(res);
  _placeholder = std::make_shared<UpdateCounter<PlaceholderItem>>(r, ax, ay);
  _parent = std::make_shared<UpdateCounter<LayoutItem>>(res);

  _placeholder->setContent(_content);
  _placeholder->setParent(_parent);
}

void PlaceholderItemTest::cleanup()
{
  _parent.reset();
  _placeholder.reset();
  _content.reset();
}

void PlaceholderItemTest::geometryUpdateWithNoContent()
{
  _placeholder->setContent(nullptr);
  _placeholder->updateGeometry();
  QCOMPARE(_placeholder->geometryUpdateCount(), 1);
  QCOMPARE(_parent->geometryUpdateCount(), 1);
}

void PlaceholderItemTest::shouldNotUseContentsGeometry()
{
  QCOMPARE_NE(_placeholder->rect(), _content->rect());
  QCOMPARE_NE(_placeholder->ax(), _content->ax());
  QCOMPARE_NE(_placeholder->ay(), _content->ay());
  QCOMPARE_EQ(_placeholder->rect(), r);
  QCOMPARE_EQ(_placeholder->ax(), ax);
  QCOMPARE_EQ(_placeholder->ay(), ay);
}

void PlaceholderItemTest::shouldScaleItemThatDoesntFit()
{
  auto cr = r.adjusted(-5, -8, 10, 16);
  auto res = std::make_shared<InvisibleResource>(cr, ax + 5, ay + 5);
  auto content = std::make_shared<LayoutItem>(std::move(res));
  _placeholder->setContent(content);
  _placeholder->updateGeometry();
  QVERIFY(content->transform().isScaling());
  const auto& csr = content->rect();
  QCOMPARE_LE(csr.width(), r.width());
  QCOMPARE_LE(csr.height(), r.height());
  QCOMPARE(csr.width() / csr.height(), cr.width() / cr.height());
}

void PlaceholderItemTest::shouldApplyContentAlignment()
{
  auto g = [](const auto& i) { return i->rect().translated(i->pos()); };

  _placeholder->setContentAlignment(Qt::AlignLeft | Qt::AlignTop);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).left(), _placeholder->rect().left());
  QCOMPARE(g(_content).top(), _placeholder->rect().top());

  _placeholder->setContentAlignment(Qt::AlignHCenter | Qt::AlignTop);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).center().x(), _placeholder->rect().center().x());
  QCOMPARE(g(_content).top(), _placeholder->rect().top());

  _placeholder->setContentAlignment(Qt::AlignRight | Qt::AlignTop);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).right(), _placeholder->rect().right());
  QCOMPARE(g(_content).top(), _placeholder->rect().top());

  _placeholder->setContentAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).left(), _placeholder->rect().left());
  QCOMPARE(g(_content).center().y(), _placeholder->rect().center().y());

  _placeholder->setContentAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).center().x(), _placeholder->rect().center().x());
  QCOMPARE(g(_content).center().y(), _placeholder->rect().center().y());

  _placeholder->setContentAlignment(Qt::AlignRight | Qt::AlignVCenter);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).right(), _placeholder->rect().right());
  QCOMPARE(g(_content).center().y(), _placeholder->rect().center().y());

  _placeholder->setContentAlignment(Qt::AlignLeft | Qt::AlignBottom);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).left(), _placeholder->rect().left());
  QCOMPARE(g(_content).bottom(), _placeholder->rect().bottom());

  _placeholder->setContentAlignment(Qt::AlignHCenter | Qt::AlignBottom);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).center().x(), _placeholder->rect().center().x());
  QCOMPARE(g(_content).bottom(), _placeholder->rect().bottom());

  _placeholder->setContentAlignment(Qt::AlignRight | Qt::AlignBottom);
  _placeholder->updateGeometry();
  QCOMPARE(g(_content).right(), _placeholder->rect().right());
  QCOMPARE(g(_content).bottom(), _placeholder->rect().bottom());
}

void PlaceholderItemTest::shouldAssignItselfAsParent()
{
  QVERIFY(_content->parent() != nullptr);
  QCOMPARE(_content->parent().get(), _placeholder.get());
}

void PlaceholderItemTest::noImplicitGeometryUpdates()
{
  QCOMPARE(_content->geometryUpdateCount(), 0);
  QCOMPARE(_placeholder->geometryUpdateCount(), 0);
  QCOMPARE(_parent->geometryUpdateCount(), 0);
  _placeholder->setContentAlignment(Qt::AlignRight | Qt::AlignBaseline);
  QCOMPARE(_placeholder->geometryUpdateCount(), 0);
  QCOMPARE(_parent->geometryUpdateCount(), 0);
  _placeholder->setContent(nullptr);
  QCOMPARE(_placeholder->geometryUpdateCount(), 0);
  QCOMPARE(_parent->geometryUpdateCount(), 0);
}

void PlaceholderItemTest::shouldHandleContentGeometryUpdates()
{
  QCOMPARE(_content->geometryUpdateCount(), 0);
  QCOMPARE(_placeholder->geometryUpdateCount(), 0);
  QCOMPARE(_parent->geometryUpdateCount(), 0);
  _content->updateGeometry();
  QCOMPARE(_content->geometryUpdateCount(), 1);
  QCOMPARE(_placeholder->geometryUpdateCount(), 1);
  QCOMPARE(_parent->geometryUpdateCount(), 1);
}

QTEST_MAIN(PlaceholderItemTest)

#include "test_placeholder.moc"
