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

#include "layout_item.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>

#include "layout_algorithm.hpp"

/**
 * @brief Layout representation
 *
 * Default-constructed object has invalid geometry and should not be used
 * in such state. At least one item must be added to the Layout.
 *
 * Layout's behavior can be customized by providing LayoutAlgorithm object.
 * By default no algorithm is set, and layout will not adjust its items'
 * geometry, just calculate bounding rect containing all of them.
 *
 * @warning Even items of this type are not supposed to be shared,
 * they must be created using `std::make_shared()`.
 *
 * @see LayoutItem
 */
class Layout : public LayoutItem {
public:
  /**
   * @brief Constructor
   *
   * Constructs Layout with given @a algorithm preset. @a algorithm can be null,
   * in such case it is equal to default constructor.
   *
   * Created object has invalid geometry in any case, as it has no items.
   *
   * @param algorithm - desired layout algorithm
   *
   * @see setAlgorithm(), addItem()
   */
  explicit Layout(std::shared_ptr<LayoutAlgorithm> algorithm = nullptr) noexcept
      : LayoutItem()
      , _algorithm(std::move(algorithm))
  {}

  /**
   * @brief Add item to layout
   *
   * Newly added item also may be a Layout, nested layouts are supported.
   *
   * @note For efficiency reasons, no geometry update happens after adding
   * the item, any geometry updates must be requested explicitly.
   *
   * @param item - item to add
   *
   * @see LayoutItem::updateGeometry()
   */
  void addItem(std::shared_ptr<LayoutItem> item)
  {
    if (!item) return;
    item->setParent(shared_from_this());
    _items.push_back(std::move(item));
  }

  /**
   * @brief Change current layout algorithm
   *
   * Sets Layout's algorithm to given one.
   *
   * Null is acceptable, in such case all child items will have their
   * initial geometry after updateGeometry() call and layout's geometry
   * will be set to corresponding bounding rect.
   *
   * @note For efficiency reasons, no geometry update happens after algorithm
   * change, any geometry updates must be requested explicitly.
   *
   * @param algorithm - desired algorithm
   *
   * @see LayoutItem::updateGeometry()
   */
  void setAlgorithm(std::shared_ptr<LayoutAlgorithm> algorithm) noexcept
  {
    _algorithm = std::move(algorithm);
  }

  /**
   * @brief Current layout items
   *
   * Returned container contains items in the same order as they were added.
   *
   * @return container with all layout items
   *
   * @see addItem()
   */
  const auto& items() const noexcept { return _items; }

  /**
   * @brief Currently used algorithm
   *
   * Provided for convenience. May be null, test before use.
   *
   * @return current LayoutAlgorithm object as shared pointer
   *
   * @see setAlgorithm()
   */
  const auto& algorithm() const noexcept { return _algorithm; }

protected:
  /**
   * @brief Reimplements LayoutItem::doUpdateGeometry()
   *
   * Updates/recalculates layout's geometry.
   * Does nothing if no items have been added.
   */
  void doUpdateGeometry() override
  {
    std::ranges::for_each(_items, [](auto&& i) noexcept { i->resetGeometry(); });

    if (_items.empty()) return;

    if (_algorithm) (*_algorithm)(_items);
    // calculate bounding rect
    QRectF r = std::accumulate(
        std::next(_items.begin()), _items.end(),
        _items.front()->geometry(),
        [](const auto& r, const auto& i) { return r | i->geometry(); }
    );
    // set it as original rect
    setInitialGeometry(r, r.width(), r.height());
  }

private:
  std::shared_ptr<LayoutAlgorithm> _algorithm;
  LayoutAlgorithm::ContainerType _items;
};
