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

  DEBUG_DRAW(debug::DrawTransformedRect, _debug_flags, p, Rect, boundingRect())
  DEBUG_DRAW(debug::DrawOriginPoint, _debug_flags, p, Ellipse, QPoint(0, 0), 2, 2)
  DEBUG_DRAW(debug::DrawHBaseline, _debug_flags, p, Line, boundingRect().left(), 0, boundingRect().right(), 0)
  DEBUG_DRAW(debug::DrawVBaseline, _debug_flags, p, Line, 0, boundingRect().top(), 0, boundingRect().bottom())

  p->setTransform(transform(), true);

  DEBUG_DRAW(debug::DrawOriginalRect, _debug_flags, p, Rect, rect())

  doDraw(p);
  p->restore();

  DEBUG_DRAW(debug::DrawGeometry, _debug_flags, p, Rect, geometry())
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

  qreal ax = 0;
  qreal ay = 0;
  if (_algorithm) std::tie(ax, ay) = _algorithm->apply(_items);
  // calculate bounding rect
  QRectF r = std::accumulate(
      std::next(_items.begin()), _items.end(),
      _items.front()->geometry(),
      [](const auto& r, const auto& i) { return r | i->geometry(); }
      );
  if (!_algorithm) std::tie(ax, ay) = std::pair{r.width(), r.height()};
  // set it as original rect
  setGeometry(r, ax, ay);
}
