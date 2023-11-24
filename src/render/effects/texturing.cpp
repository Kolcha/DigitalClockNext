#include "texturing.hpp"

#include "core/hasher.hpp"
#include "render/state_guard_qpainter.hpp"

TexturingEffect::TexturingEffect(QBrush brush, bool stretch)
  : _brush(std::move(brush))
  , _stretch(stretch)
{
  updateHash(hasher(_brush, _stretch));
}

void TexturingEffect::setBrush(QBrush brush)
{
  _brush = std::move(brush);
  updateHash(hasher(_brush, _stretch));
}

void TexturingEffect::setStretch(bool stretch)
{
  _stretch = stretch;
  updateHash(hasher(_brush, _stretch));
}

void TexturingEffect::simpleApply(QPainter* p, const QRectF& r) const
{
  StateGuard _(p);
  p->setCompositionMode(QPainter::CompositionMode_SourceIn);
  p->setPen(Qt::NoPen);
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(r, tx, tx.rect());
    return;
  }
  p->setBrush(_brush);
  p->drawRect(r.adjusted(-0.51, -0.51, 0.51, 0.51));  // ?
}
