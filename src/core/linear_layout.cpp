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

  qreal dpos = 0;
  qreal min_coord = (items.front()->boundingRect().*_orientation->minCoord)();
  qreal max_coord = (items.front()->boundingRect().*_orientation->maxCoord)();
  qreal max_op_adv = (*items.front().*_opposite_or->advance)();
  for (const auto& item : items) {
    const auto& br = item->boundingRect();
    min_coord = std::min(min_coord, (br.*_orientation->minCoord)());
    max_coord = std::max(max_coord, (br.*_orientation->maxCoord)());
    max_op_adv = std::max(max_op_adv, (*item.*_opposite_or->advance)());
    item->setPos((*_orientation->position)(dpos));
    dpos += (*item.*_orientation->advance)() + _spacing;
  }

  Q_ASSERT(max_coord >= min_coord);

  for (const auto& item : items) {
    auto geometry = item->boundingRect().translated(item->pos());
    (geometry.*_orientation->setMinCoord)(min_coord);
    (geometry.*_orientation->setMaxCoord)(max_coord);
    item->setGeometry(std::move(geometry));
  }

  qreal ax = (items.back()->pos().*_orientation->coord)() +
             (*items.back().*_orientation->advance)() -
             (items.front()->pos().*_orientation->coord)();
  qreal ay = max_op_adv;
  if (_orientation == &vertical) std::swap(ax, ay);
  return {ax, ay};
}
