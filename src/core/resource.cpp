/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "resource.hpp"

#include <QPainter>
#include <QPixmapCache>

void CachedResource::draw(QPainter* p)
{
  if (rect().isEmpty())
    return;

  p->save();
  auto ext_tr = p->transform();
  auto br = p->transform().mapRect(rect());
  auto sz = (br.size() * p->device()->devicePixelRatioF()).toSize();

  QString key = QString("%1_%2x%3")
                .arg(cacheKey())
                .arg(sz.width()).arg(sz.height());
  QPixmap pxm;

  if (!QPixmapCache::find(key, &pxm)) {
    pxm = QPixmap(sz);
    pxm.setDevicePixelRatio(p->device()->devicePixelRatioF());
    pxm.fill(Qt::transparent);
    {
      QPainter pp(&pxm);
      pp.setRenderHints(p->renderHints());
      pp.translate(-br.topLeft());
      pp.setTransform(ext_tr, true);
      ResourceDecorator::draw(&pp);
    }
    QPixmapCache::insert(key, pxm);
  }
  p->resetTransform();
  p->translate(br.topLeft());
  p->drawPixmap(0, 0, pxm);
  p->restore();
}