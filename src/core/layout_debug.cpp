/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
