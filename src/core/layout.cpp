/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023-2024  Nick Korotysh <nick.korotysh@gmail.com>

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
