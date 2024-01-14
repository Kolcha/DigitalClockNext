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

#include "linear_layout.hpp"

#include <algorithm>

std::pair<qreal, qreal> LinearLayout::apply(const ContainerType& items) const
{
  if (items.empty()) return {0, 0};

  auto cdir = _orientation;
  auto odir = _orientation == &horizontal ? &vertical : &horizontal;

  auto prev = items.begin();
  (*prev)->setPos({0, 0});  // the first item is always at (0,0)

  qreal min_coord = ((*prev)->boundingRect().*odir->minCoord)();
  qreal max_coord = ((*prev)->boundingRect().*odir->maxCoord)();
  qreal max_adv = (**prev.*odir->advance)();

  for (auto iter = std::next(prev); iter != items.end(); ++iter) {
    const auto& item = *iter;
    const auto& br = item->boundingRect();
    // values used for items' geometry calculation
    min_coord = std::min(min_coord, (br.*odir->minCoord)());
    max_coord = std::max(max_coord, (br.*odir->maxCoord)());
    max_adv = std::max(max_adv, (*item.*odir->advance)());
    // positioning
    qreal dpos = ((*prev)->pos().*cdir->coord)();
    if (_ignore_advance) {
      dpos += ((*prev)->boundingRect().*cdir->maxCoord)();
      dpos -= ((*iter)->boundingRect().*cdir->minCoord)();
    } else {
      const auto& i = cdir == &horizontal ? *prev : *iter;
      dpos += (*i.*cdir->advance)();
    }
    item->setPos((*cdir->position)(dpos + _spacing));
    // current -> previous
    prev = iter;
  }

  Q_ASSERT(max_coord >= min_coord);
  // items' geometry calculation
  for (const auto& item : items) {
    auto geometry = item->boundingRect().translated(item->pos());
    (geometry.*odir->setMinCoord)(min_coord);
    (geometry.*odir->setMaxCoord)(max_coord);
    item->setGeometry(std::move(geometry));
  }

  // layout's ax/ay calculation
  qreal ax, ay;
  if (_orientation == &horizontal) {
    ax = items.back()->pos().x() -
         items.front()->pos().x() +
         items.back()->advanceX();    // last item
    ay = max_adv;
  } else {
    ay = items.back()->pos().y() -
         items.front()->pos().y() +
         items.front()->advanceY();   // first item
    ax = max_adv;
  }
  return {ax, ay};
}
