/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "composite.hpp"

#include <QPainter>
#include <QPixmap>

#include "hasher.hpp"
#include "state_guard_qpainter.hpp"

void CompositeEffect::addEffect(std::shared_ptr<Effect> effect)
{
  _effects.push_back(std::move(effect));
  updateHash(hasher(_effects));
}

void CompositeEffect::clearEffects()
{
  _effects.clear();
  updateHash(hasher(_effects));
}

void CompositeEffect::apply(QPainter* p, const QRectF& r, RenderItemFn render_item) const
{
  StateGuard _(p);
  QPixmap buffer(p->device()->width(), p->device()->height());
  buffer.setDevicePixelRatio(p->device()->devicePixelRatioF());
  buffer.fill(Qt::transparent);
  {
    QPainter pp(&buffer);
    pp.setRenderHints(p->renderHints());
    pp.setTransform(p->transform());
    for (const auto& effect : _effects)
      (*effect)(&pp, r, render_item);
  }
  p->resetTransform();
  p->drawPixmap(0, 0, buffer);
}
