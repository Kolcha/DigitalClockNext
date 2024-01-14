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

#include "layout_debug.hpp"

#include <QPainter>

namespace debug {

QPen configurePen(LayoutDebugFlag f)
{
  QPen p(Qt::black, 2.0);
  p.setCosmetic(true);
  switch (f) {
    case DrawOriginalRect:
      p.setColor(QColor(96, 0, 16, 128));
      break;
    case DrawTransformedRect:
      p.setColor(QColor(0, 128, 0, 128));
      break;
    case DrawGeometry:
      p.setColor(QColor(0, 16, 96, 128));
      break;
    case DrawOriginPoint:
      p.setColor(Qt::white);
      [[fallthrough]];
    case DrawHBaseline:
    case DrawVBaseline:
      p.setStyle(Qt::DashLine);
      break;
  }
  return p;
}

QBrush configureBrush(LayoutDebugFlag f)
{
  QBrush b(Qt::white);
  switch (f) {
    case DrawOriginalRect:
      b.setColor(QColor(240, 0, 64, 32));
      break;
    case DrawTransformedRect:
      b.setColor(QColor(0, 255, 16, 32));
      break;
    case DrawGeometry:
      b.setColor(QColor(0, 96, 224, 32));
      break;
    case DrawOriginPoint:
    case DrawHBaseline:
    case DrawVBaseline:
      b = Qt::NoBrush;
      break;
  }
  return b;
}

PainterSetup::PainterSetup(QPainter* p, LayoutDebugFlag f)
  : _p(*p)
{
  _p.save();
  _p.setPen(configurePen(f));
  _p.setBrush(configureBrush(f));
}

PainterSetup::~PainterSetup()
{
  _p.restore();
}

} // namespace debug
