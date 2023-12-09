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

#include "texturing.hpp"

#include "hasher.hpp"
#include "state_guard_qpainter.hpp"

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
