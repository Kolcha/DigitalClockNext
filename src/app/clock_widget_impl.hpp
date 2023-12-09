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

#pragma once

#include <memory>

#include "rendering.hpp"
#include "state_guard_qpainter.hpp"
#include "clock_skin.hpp"

// TODO: what about plugins' layouts? for now skin assumes only time
// TODO: handle plugins' layouts using Qt' stuff - widgets may be clickable
class ClockWidget final {
public:
  ClockWidget(const QDateTime& dt, std::shared_ptr<ClockSkin> skin)
    : _skin(skin)
  {
    // initial value must be supplied to build layout
    setDateTime(dt);
  }

  void setDateTime(const QDateTime& dt)
  {
    _item = _skin->process(dt);
  }

  void animateSeparator()
  {
    _skin->animateSeparator();
  }

  void render(QPainter* p) const
  {
    if (!p) return;
    StateGuard _(p);
    p->translate(-_item->geometry().topLeft());
    _item->render(p);
  }

  QRectF geometry() const
  {
    return _item->geometry();
  }

private:
  std::shared_ptr<ClockSkin> _skin;
  std::shared_ptr<ClockRenderable> _item;
};
