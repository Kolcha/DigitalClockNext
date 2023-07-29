#include "texturing_effect.hpp"

#include "core/hasher.hpp"
#include "render/state_guard_qpainter.hpp"

TexturingEffect::TexturingEffect(QBrush brush)
  : _brush(std::move(brush))
{
  updateHash(hasher(_brush));
}

void TexturingEffect::setBrush(QBrush brush)
{
  _brush = std::move(brush);
  updateHash(hasher(_brush));
}

void TexturingEffect::simpleApply(QPainter* p, const QRectF& r) const
{
  StateGuard _(p);
  p->setCompositionMode(QPainter::CompositionMode_SourceIn);
  p->setPen(Qt::NoPen);
  p->setBrush(_brush);
  p->drawRect(r.adjusted(-0.51, -0.51, 0.51, 0.51));  // ?
}
