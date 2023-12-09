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

#include "char_renderable.hpp"

#include <QFontMetricsF>
#include <QString>

#include "hasher.hpp"
#include "state_guard_qpainter.hpp"

QCharRenderable::QCharRenderable(QChar ch, const QFont& font)
  : _ch(ch)
  , _font(font)
  , _hash(hasher(ch, font))
{
  QFontMetricsF fmf(font);
  _br = fmf.boundingRect(ch);
  _ax = fmf.horizontalAdvance(ch);
  _ay = fmf.lineSpacing();
}

void QCharRenderable::render(QPainter* p)
{
  StateGuard _(p);
  p->setFont(_font);
  p->drawText(QPointF(0., 0.), QString(_ch));
}
