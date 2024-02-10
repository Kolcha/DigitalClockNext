/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "layout.hpp"

#include <functional>
#include <numeric>

#include <QPainter>

#include "layout_debug.hpp"

namespace {

using DebugContext = ::debug::LayoutDebug;

class DebugResource final : public ResourceDecorator {
public:
  static auto decorate(std::shared_ptr<Resource> res, DebugContext ctx)
  {
    // avoid double decoration, re-use the same debug resource
    if (auto dres = std::dynamic_pointer_cast<DebugResource>(res))
      return dres;

    auto dres = std::make_shared<DebugResource>(std::move(res));
    dres->_ctx = std::move(ctx);
    return dres;
  }

  void draw(QPainter* p) override
  {
    ResourceDecorator::draw(p);

    DEBUG_DRAW(debug::DrawOriginalRect, _ctx, p, Rect, rect());
    DEBUG_DRAW(debug::DrawOriginPoint, _ctx, p, Ellipse, QPoint(0, 0), 2, 2);
    DEBUG_DRAW(debug::DrawHBaseline, _ctx, p, Line, rect().left(), 0, rect().right(), 0);
    DEBUG_DRAW(debug::DrawVBaseline, _ctx, p, Line, 0, rect().top(), 0, rect().bottom());
  }

protected:
  using ResourceDecorator::ResourceDecorator;

private:
  DebugContext _ctx;
};

auto item_context() noexcept
{
  auto raw_value = qEnvironmentVariableIntValue(debug::ItemDebugFlagsVar);
  return static_cast<debug::LayoutDebug>(raw_value);
}

auto layout_context() noexcept
{
  auto raw_value = qEnvironmentVariableIntValue(debug::LayoutDebugFlagsVar);
  return static_cast<debug::LayoutDebug>(raw_value);
}

} // namespace

LayoutItem::LayoutItem(std::shared_ptr<Resource> res)
  : _res(DebugResource::decorate(std::move(res), item_context()))
{
  Q_ASSERT(_res);
  updateCachedGeometry();
}

void LayoutItem::updateGeometry()
{
  doUpdateGeometry();
  updateCachedGeometry();

  if (auto parent = _parent.lock())
    parent->updateGeometry();
}

void LayoutItem::setResizeEnabled(bool enabled)
{
  _resize_enabled = enabled;
  if (!enabled) _ks = 1.0;
  updateCachedGeometry();
}

void LayoutItem::resize(qreal l, Qt::Orientation o)
{
  Q_ASSERT(_resize_enabled);
  if (o == Qt::Horizontal)
    _ks = l / _rect.width();
  else
    _ks = l / _rect.height();
  updateCachedGeometry();
}

void LayoutItem::updateCachedGeometry()
{
  _rect = _transform.mapRect(_res->rect());
  _ax = _transform.map(QLineF(0, 0, _res->advanceX(), 0)).dx();
  _ay = _transform.map(QLineF(0, 0, 0, _res->advanceY())).dy();

  if (_resize_enabled) {
    _rect = QRectF(_rect.topLeft() * _ks, _rect.size() * _ks);
    _ax *= _ks;
    _ay *= _ks;
  }
}

Layout::Layout() : Layout(std::make_shared<Layout::LayoutResource>())
{
}

Layout::Layout(std::shared_ptr<LayoutResource> res)
  : LayoutItem(DebugResource::decorate(res, layout_context()))
  , _res(std::move(res))
{
  Q_ASSERT(_res);
}

void Layout::LayoutResource::draw(QPainter* p)
{
  for (const auto& item : _items) {
    p->save();
    p->translate(item->pos());
    p->setTransform(item->transform(), true);
    item->resource()->draw(p);
    p->restore();
  }
}

size_t Layout::LayoutResource::cacheKey() const
{
  return std::transform_reduce(
        _items.begin(), _items.end(),
        static_cast<size_t>(0),
        std::bit_xor{},
        [](const auto& item) { return item->resource()->cacheKey(); }
  );
}

void Layout::LayoutResource::updateGeometry(qreal ax, qreal ay)
{
  if (_items.empty()) return;

  _rect = std::transform_reduce(
            _items.begin(), _items.end(),
            _items.front()->rect().translated(_items.front()->pos()),
            std::bit_or{},
            [](const auto& i) { return i->rect().translated(i->pos()); }
  );

  _ax = ax;
  _ay = ay;
}

void PlaceholderItem::doUpdateGeometry()
{
  const auto& item = _res->content();
  if (!item) return;

  if (item->rect().width() > _res->rect().width() ||
      item->rect().height() > _res->rect().height()) {
    auto os = item->rect().size();
    auto ss = os.scaled(_res->rect().size(), Qt::KeepAspectRatio);
    auto kx = ss.width() / os.width();
    auto ky = ss.height() / os.height();
    item->setTransform(item->transform().scale(kx, ky));
  }

  const auto& g = _res->rect();
  const auto& r = item->rect().translated(item->pos());
  auto halign = _alignment & Qt::AlignHorizontal_Mask;
  auto valign = _alignment & Qt::AlignVertical_Mask;
  qreal dx = 0;
  qreal dy = 0;
  if (halign == Qt::AlignLeft) dx = g.left() - r.left();
  if (halign == Qt::AlignHCenter) dx = g.center().x() - r.center().x();
  if (halign == Qt::AlignRight) dx = g.right() - r.right();
  if (valign == Qt::AlignTop) dy = g.top() - r.top();
  if (valign == Qt::AlignVCenter) dy = g.center().y() - r.center().y();
  if (valign == Qt::AlignBottom) dy = g.bottom() - r.bottom();

  item->setPos(item->pos() + QPointF(dx, dy));
}

void PlaceholderItem::PlaceholderResource::draw(QPainter* p)
{
  if (!_item) return;

  p->save();
  p->translate(_item->pos());
  p->setTransform(_item->transform(), true);
  _item->resource()->draw(p);
  p->restore();
}
