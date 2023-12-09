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

#include "rendering.hpp"

#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>

#include "hasher.hpp"
#include "state_guard_qpainter.hpp"

void SkinElement::render(QPainter* p)
{
  if (!isVisible() || item()->rect().isEmpty())
    return;

  StateGuard _(p);
  p->setTransform(item()->transform(), true);
  auto br = p->transform().mapRect(item()->rect());
  auto sz = (br.size() * p->device()->devicePixelRatioF()).toSize();

  QString key;
  QPixmap pxm;

  if (auto h = hash(); h != Hashable::Invalid)
    key = QString("%1_%2x%3").arg(h).arg(sz.width()).arg(sz.height());

  if (key.isEmpty() || !QPixmapCache::find(key, &pxm)) {
    pxm = QPixmap(sz);
    pxm.setDevicePixelRatio(p->device()->devicePixelRatioF());
    pxm.fill(Qt::transparent);
    {
      QPainter pp(&pxm);
      pp.setRenderHints(p->renderHints());
      pp.translate(-br.topLeft());
      pp.setTransform(p->transform(), true);
      _effects.empty() ? renderImpl(&pp) : applyEffects(&pp);
    }
    QPixmapCache::insert(key, pxm);
  }

  p->resetTransform();
  p->translate(br.topLeft());
  p->drawPixmap(0, 0, pxm);
}

size_t SimpleSkinElement::hash() const noexcept
{
  // it is assumed that:
  // - all effects must have valid hash implementation
  // - all resources must have valid hash implementation
  return _res->hash() ^ hasher(effects());
}
