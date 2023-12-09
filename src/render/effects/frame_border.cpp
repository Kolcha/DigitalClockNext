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

#include "frame_border.hpp"

#include "hasher.hpp"
#include "state_guard_qpainter.hpp"

FrameBorderEffect::FrameBorderEffect(QPen p)
  : _pen(std::move(p))
{
  updateHash(hasher(_pen));
}

void FrameBorderEffect::setPen(QPen p)
{
  _pen = std::move(p);
  updateHash(hasher(_pen));
}

void FrameBorderEffect::simpleApply(QPainter* p, const QRectF& r) const
{
  StateGuard _(p);
  p->setPen(_pen);
  p->setBrush(Qt::NoBrush);
  p->drawRect(r);
}
