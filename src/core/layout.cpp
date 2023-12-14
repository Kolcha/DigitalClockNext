#include "layout.hpp"

#include <algorithm>
#include <numeric>

#include <QPainter>

#include "hasher.hpp"

void GlyphBase::draw(QPainter* p)
{
  if (!isVisible()) return;

  p->save();
  p->setTransform(transform(), true);
  doDraw(p);
  p->restore();
}

void GlyphBase::updateGeometry()
{
  doUpdateGeometry();

  if (auto p = parent())
    p->updateGeometry();
}

void GlyphBase::updateCachedGeometry()
{
  _curr_geom.setRect(_transform.mapRect(_init_geom.rect()));
  _curr_geom.setAdvanceX(_transform.map(QLineF(0, 0, _init_geom.advanceX(), 0)).dx());
  _curr_geom.setAdvanceY(_transform.map(QLineF(0, 0, 0, _init_geom.advanceY())).dy());
}

size_t CompositeGlyph::cacheKey() const
{
  return hasher(_items);
}

void CompositeGlyph::doUpdateGeometry()
{
  std::ranges::for_each(_items, [](auto&& i) { i->resetGeometry(); });

  if (_items.empty()) return;

  if (_algorithm) _algorithm->apply(_items);
  // calculate bounding rect
  QRectF r = std::accumulate(
      std::next(_items.begin()), _items.end(),
      _items.front()->geometry(),
      [](const auto& r, const auto& i) { return r | i->geometry(); }
      );
  // set it as original rect
  setGeometry(r, r.width(), r.height());
}
