/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
