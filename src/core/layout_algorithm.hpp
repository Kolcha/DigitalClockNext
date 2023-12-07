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
#include <vector>

#include "layout_item.hpp"

/**
 * @brief Base class for layout algorithms
 *
 * Provides an interface for layout algorithms implementations.
 *
 * Even this is not a strict requirement, it is advised that algorithms
 * should be designed to be shareable between many layouts.
 *
 * @see Layout::setAlgorithm()
 */
class LayoutAlgorithm {
public:
  /// Containter type used to represent items in the Layout
  using ContainerType = std::vector<std::shared_ptr<LayoutItem>>;

  /**
   * @brief Virtual destructor
   *
   * Just a necessary thing.
   */
  virtual ~LayoutAlgorithm() = default;

  /**
   * @brief The "entry point"
   *
   * It is called by Layout each time its geometry needs to be updated.
   *
   * It is guaranteed that items in the container are in the same order
   * as they have been added to the corresponding layout, and all items
   * always have their initial geometry and no transform, regardless of
   * how many times layout's geometry was updated.
   *
   * @param items - layout's items
   */
  void operator ()(const ContainerType& items) const { apply(items); }

private:
  /// layout algorithm implementation, derived class must implement it
  virtual void apply(const ContainerType& items) const = 0;
};
