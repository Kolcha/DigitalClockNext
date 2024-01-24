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

#pragma once

#include <QFlags>

class QPainter;

namespace debug {

enum LayoutDebugFlag {
  DrawOriginalRect    = 0x0001,
  DrawTransformedRect = 0x0002,
  DrawGeometry        = 0x0004,
  DrawOriginPoint     = 0x0008,
  DrawHBaseline       = 0x0010,
  DrawVBaseline       = 0x0020,
};

Q_DECLARE_FLAGS(LayoutDebug, LayoutDebugFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(LayoutDebug)

class PainterSetup {
public:
  PainterSetup(QPainter* p, LayoutDebugFlag f);
  ~PainterSetup();

private:
  QPainter& _p;
};

static constexpr const char* const ItemDebugFlagsVar =
    "DIGITAL_CLOCK_NEXT_DEBUG_ITEM_FLAGS";
static constexpr const char* const LayoutDebugFlagsVar =
    "DIGITAL_CLOCK_NEXT_DEBUG_LAYOUT_FLAGS";

} // namespace debug

#define DEBUG_DRAW(flag, flags, painter, shape, ...)  \
  if (flags & flag) {                                 \
    debug::PainterSetup _(painter, flag);             \
    painter->draw##shape(__VA_ARGS__);                \
  }
