#include "effects.hpp"

#include <QPainter>

void NewSurfaceEffect::draw(QPainter* p)
{
  p->save();
  QSize sz(p->device()->width(), p->device()->height());
  sz *= p->device()->devicePixelRatioF();
  QPixmap buffer(sz);
  buffer.setDevicePixelRatio(p->device()->devicePixelRatioF());
  buffer.fill(Qt::transparent);
  {
    QPainter pp(&buffer);
    pp.setRenderHints(p->renderHints());
    pp.setTransform(p->transform());
    GlyphDecorator::draw(&pp);
  }
  p->resetTransform();
  p->drawPixmap(0, 0, buffer);
  p->restore();
}

void TexturingEffect::draw(QPainter* p)
{
  GlyphDecorator::draw(p);
  p->save();
  p->setCompositionMode(QPainter::CompositionMode_SourceIn);
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(geometry(), tx, tx.rect());
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(_brush);
    p->drawRect(geometry());
  }
  p->restore();
}

void BackgroundEffect::draw(QPainter* p)
{
  p->save();
  p->setCompositionMode(QPainter::CompositionMode_SourceOver);
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(geometry(), tx, tx.rect());
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(_brush);
    p->drawRect(geometry());
  }
  p->restore();
  GlyphDecorator::draw(p);
}
