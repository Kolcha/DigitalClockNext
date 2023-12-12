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

#include "error_skin.hpp"

#include <QPainter>

void ErrorMessage::draw(QPainter* p)
{
  using namespace Qt::Literals::StringLiterals;
  p->fillRect(rect(), Qt::black);
  auto fnt = p->font();
  fnt.setPointSize(14);
  p->setFont(fnt);
  p->setPen(QColor(112, 96, 240));
  auto r = rect();
  r.setHeight(r.height() / 4.0);
  p->drawText(r, Qt::AlignCenter, u"Digital Clock Next"_s);
  if (_visible) {
    p->setPen(Qt::red);
    fnt.setPointSize(48);
    p->setFont(fnt);
    p->drawText(rect(), Qt::AlignCenter, u"ERROR"_s);
  }
  p->setPen(Qt::white);
  fnt.setPointSize(12);
  p->setFont(fnt);
  r.translate(0, 3 * r.height());
  p->drawText(r, Qt::AlignCenter, u"skin has not been loaded"_s);
}
