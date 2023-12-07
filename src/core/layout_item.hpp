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

#include <QRectF>
#include <QTransform>

#include "item_geometry.hpp"

/**
 * @brief Generic Layout Item
 *
 * Keeps track item's geometry according to its transform.
 * May built hierarchical tree-like structures.
 *
 * By default item is not transformed in any way, so all
 * geometry-related methods returns exactly the same values as
 * were passed to the constructor.
 *
 * @warning Even items of this type are not supposed to be shared,
 * they must be created using `std::make_shared()`.
 *
 * @see parent(), transform(), geometry()
 */
class LayoutItem : public std::enable_shared_from_this<LayoutItem> {
public:
  /**
   * @brief Constructor
   *
   * Constructs LayoutItem with given initial geometry.
   *
   * @param r - initial bounding rect
   * @param ax - horizontal distance to the next item
   * @param ay - vertical distance to the next item
   * @param p - parent item (optional)
   *
   * @a ax and @a ay are distances between (0,0) of this item and
   * (0,0) of the next item following this one in corresponding direction.
   * If not applicable, they should be set to @a r.width() and @a r.height().
   */
  LayoutItem(QRectF r, qreal ax, qreal ay, std::weak_ptr<LayoutItem> p = {})
  {
    setInitialGeometry(std::move(r), ax, ay);
    setParent(std::move(p));
  }

  /**
   * @brief Virtual destructor
   *
   * Just a necessary thing.
   */
  virtual ~LayoutItem() = default;

  /**
   * @brief Original (unmodified) item's geometry
   * @return bounding rect before applying transform
   */
  inline const QRectF& rect() const noexcept { return _init_geom.rect(); }

  /**
   * @brief Item's geometry
   * @return item's bounding rect after applying the transform
   */
  inline const QRectF& geometry() const noexcept { return _curr_geom.rect(); }
  /**
   * @brief Distance to the next item in horizontal direction
   * @return transformed distance to the next item
   */
  inline qreal advanceX() const noexcept { return _curr_geom.advanceX(); }
  /**
   * @brief Distance to the next item in vertical direction
   * @return transformed distance to the next item
   */
  inline qreal advanceY() const noexcept { return _curr_geom.advanceY(); }

  /**
   * @brief Item's transform matrix
   * @return current transform, identity matrix by default
   * @see setTransform()
   */
  inline const QTransform& transform() const noexcept { return _transform; }

  /**
   * @brief Item's parent
   * @return parent item or nullptr if no parent was set
   * @see setParent()
   */
  std::shared_ptr<LayoutItem> parent() const noexcept { return _parent.lock(); }

  /**
   * @brief Change item's parent
   *
   * Only reference is stored, this (child) item doesn't get destroyed
   * on parent item destruction, it is caller's responsibility to manage
   * the lifetime of both parent and current (child) objects.
   *
   * @param p - new parent item
   */
  void setParent(std::weak_ptr<LayoutItem> p) noexcept { _parent = std::move(p); }

  /**
   * @brief Change item's transform
   *
   * Assigns given transform and updates item's geometry accroding to it.
   *
   * If called multiple times, any previous transformation is discarded,
   * new geometry is calculated only based on initial item's geometry.
   *
   * @param t - new transform
   *
   * @note Even non-affine transformations are supported, prefer to avoid them.
   * @note Geometry changes are not propagated, call updateGeometry() explicitly.
   * @see geometry(), advanceX(), advanceY()
   */
  void setTransform(QTransform t)
  {
    _transform = std::move(t);
    updateCachedGeometry();
  }

  /**
   * @brief Recalculate item's geometry
   *
   * Explicit request to update item's geometry according to current transform.
   *
   * Item's geometry change is propagated to the parent item if such was set.
   *
   * @note May be expensive, should be avoided if no strict reason to call it.
   * @see setTransform()
   */
  void updateGeometry()
  {
    doUpdateGeometry();

    if (auto parent = _parent.lock())
      parent->updateGeometry();
  }

  /**
   * @brief Reset item's geometry
   *
   * Sets current item's geometry to its initial geometry. Transform is also reset.
   * @note Geometry changes are not propagated, call updateGeometry() explicitly.
   * @see setInitialGeometry()
   */
  void resetGeometry() noexcept
  {
    _curr_geom = _init_geom;
    _transform = QTransform();
  }

  LayoutItem& operator=(LayoutItem &&other) noexcept = default;
  LayoutItem& operator=(const LayoutItem &) noexcept = delete;
  LayoutItem(LayoutItem&& other) noexcept = default;
  LayoutItem(const LayoutItem& other) noexcept = delete;

protected:
  /**
   * @brief Change initial item's geometry
   *
   * This is the possibility for derived classes to update (or just define)
   * item's geometry dynamically.
   *
   * @param r - initial bounding rect
   * @param ax - horizontal distance to the next item
   * @param ay - vertical distance to the next item
   */
  void setInitialGeometry(QRectF r, qreal ax, qreal ay)
  {
    _init_geom.setRect(std::move(r));
    _init_geom.setAdvance(ax, ay);
    updateCachedGeometry();
  }

  /**
   * @brief Protected default constructor
   *
   * Just to allow derived classes to delay initialization. It is no supposed
   * to create items with invalid geometry (default-initialized), so this
   * possibility should be used carefully.
   *
   * It exists to leverage the possibility provided by setInitialGeometry()
   * @see setInitialGeometry()
   */
  LayoutItem() = default;

  /**
   * @brief Geometry update implementation
   *
   * Calculates geometry based on initial item's geometry and current transform.
   */
  virtual void doUpdateGeometry() { updateCachedGeometry(); }

private:
  /// Updates cached geometry
  void updateCachedGeometry()
  {
    _curr_geom.setRect(_transform.mapRect(_init_geom.rect()));
    _curr_geom.setAdvanceX(_transform.map(QLineF(0, 0, _init_geom.advanceX(), 0)).dx());
    _curr_geom.setAdvanceY(_transform.map(QLineF(0, 0, 0, _init_geom.advanceY())).dy());
  }

private:
  ItemGeometry _init_geom;
  ItemGeometry _curr_geom;
  QTransform _transform;
  std::weak_ptr<LayoutItem> _parent;
};
