#include "layout.hpp"

#include <numeric>

#include <QPainter>

#include "hasher.hpp"

void GlyphBase::draw(QPainter* p)
{
  if (!isVisible()) return;

  p->save();
  p->translate(pos());
  auto br = boundingRect().translated(pos());
  auto halign = _alignment & Qt::AlignHorizontal_Mask;
  auto valign = _alignment & Qt::AlignVertical_Mask;
  qreal dx = 0;
  qreal dy = 0;
  if (halign == Qt::AlignLeft) dx = _geometry.left() - br.left();
  if (halign == Qt::AlignHCenter) dx = _geometry.center().x() - br.center().x();
  if (halign == Qt::AlignRight) dx = _geometry.right() - br.right();
  if (valign == Qt::AlignTop) dy = _geometry.top() - br.top();
  if (valign == Qt::AlignVCenter) dy = _geometry.center().y() - br.center().y();
  if (valign == Qt::AlignBottom) dy = _geometry.bottom() - br.bottom();
  p->translate(dx, dy);
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
  _geometry = _curr_geom.rect().translated(_pos);
}

size_t CompositeGlyph::cacheKey() const
{
  return hasher(_items);
}

void CompositeGlyph::doUpdateGeometry()
{
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
