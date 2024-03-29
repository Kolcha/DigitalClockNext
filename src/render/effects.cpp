/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "effects.hpp"

#include <QPainter>

void NewSurfaceDecorator::draw(QPainter* p)
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
    ResourceDecorator::draw(&pp);
  }
  p->resetTransform();
  p->drawPixmap(0, 0, buffer);
  p->restore();
}

Effect::ResourcePtr NewSurfaceEffect::decorate(ResourcePtr res)
{
  return std::make_shared<NewSurfaceDecorator>(std::move(res));
}

void TexturingDecorator::draw(QPainter* p)
{
  ResourceDecorator::draw(p);
  p->save();
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

Effect::ResourcePtr TexturingEffect::decorate(ResourcePtr res)
{
  auto dres = std::make_shared<TexturingDecorator>(std::move(res));
  dres->setBrush(brush());
  dres->setStretch(stretch());
  return dres;
}

void BackgroundDecorator::draw(QPainter* p)
{
  p->save();
  p->setCompositionMode(QPainter::CompositionMode_SourceOver);
  if (auto tx = _brush.texture(); !tx.isNull() && _stretch) {
    p->drawPixmap(rect(), tx, tx.rect());
  } else {
    p->setPen(Qt::NoPen);
    p->setBrush(_brush);
    p->drawRect(rect());
  }
  p->restore();
  ResourceDecorator::draw(p);
}

Effect::ResourcePtr BackgroundEffect::decorate(ResourcePtr res)
{
  auto dres = std::make_shared<BackgroundDecorator>(std::move(res));
  dres->setBrush(brush());
  dres->setStretch(stretch());
  return dres;
}
