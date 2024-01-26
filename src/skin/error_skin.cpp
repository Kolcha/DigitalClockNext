/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
