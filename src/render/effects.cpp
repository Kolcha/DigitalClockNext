/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023-2024  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    ResourceDecorator::draw(&pp);
  }
  p->resetTransform();
  p->drawPixmap(0, 0, buffer);
  p->restore();
}

void TexturingEffect::draw(QPainter* p)
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

void BackgroundEffect::draw(QPainter* p)
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
