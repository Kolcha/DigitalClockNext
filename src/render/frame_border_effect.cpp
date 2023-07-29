#include "frame_border_effect.hpp"

#include "core/hasher.hpp"
#include "render/state_guard_qpainter.hpp"

FrameBorderEffect::FrameBorderEffect(QPen p)
  : _pen(std::move(p))
{
  updateHash(hasher(_pen));
}

void FrameBorderEffect::setPen(QPen p)
{
  _pen = std::move(p);
  updateHash(hasher(_pen));
}

void FrameBorderEffect::simpleApply(QPainter* p, const QRectF& r) const
{
  StateGuard _(p);
  p->setPen(_pen);
  p->setBrush(Qt::NoBrush);
  p->drawRect(r);
}
