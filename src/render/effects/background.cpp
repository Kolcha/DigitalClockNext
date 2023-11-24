#include "background.hpp"

#include "core/hasher.hpp"

BackgroundEffect::BackgroundEffect(QBrush brush, bool stretch)
  : _brush(std::move(brush))
  , _stretch(stretch)
{
  updateHash(hasher(_brush, _stretch));
}

void BackgroundEffect::setBrush(QBrush brush)
{
  _brush = std::move(brush);
  updateHash(hasher(_brush, _stretch));
}

void BackgroundEffect::setStretch(bool stretch)
{
  _stretch = stretch;
  updateHash(hasher(_brush, _stretch));
}

void BackgroundEffect::simpleApply(QPainter* p, const QRectF& r) const
{
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(r, tx, tx.rect());
    return;
  }
  p->fillRect(r, _brush);
}
