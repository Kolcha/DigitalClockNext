#include "effects.hpp"

#include <QPainter>

void BorderEffect::draw(QPainter* p)
{
  GlyphDecorator::draw(p);
  p->save();
  p->setTransform(transform(), true);
  QPen pen(_color, _width);
  pen.setCosmetic(true);
  p->setPen(pen);
  p->setBrush(Qt::NoBrush);
  p->drawRect(rect());
  p->drawLine(rect().topLeft(), rect().bottomRight());
  p->drawLine(rect().topRight(), rect().bottomLeft());
  p->restore();
}

void BaseLineEffect::draw(QPainter* p)
{
  GlyphDecorator::draw(p);
  p->save();
  p->setTransform(transform(), true);
  QPen pen(_color, _width);
  pen.setCosmetic(true);
  p->setPen(pen);
  p->setBrush(Qt::NoBrush);
  p->drawLine(QPointF(rect().left(), 0.0), QPointF(rect().right(), 0.0));
  p->restore();
}

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
  p->setTransform(transform(), true);
  p->setCompositionMode(QPainter::CompositionMode_SourceIn);
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(rect(), tx, tx.rect());
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(_brush);
    p->drawRect(rect());
  }
  p->restore();
}

void BackgroundEffect::draw(QPainter* p)
{
  p->save();
  p->setTransform(transform(), true);
  p->setCompositionMode(QPainter::CompositionMode_Source);
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(rect(), tx, tx.rect());
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(_brush);
    p->drawRect(rect());
  }
  p->restore();
  GlyphDecorator::draw(p);
}
