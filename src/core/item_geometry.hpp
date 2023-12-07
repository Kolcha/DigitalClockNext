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

#include <QRectF>

/**
 * @brief Represents item's geometry
 *
 * Just a convenient class that ties bounding rectangle and distances to the next item
 * together, and provides a lot of convenient ways to initialize them.
 *
 * No any sanity checks or values validation is implemeted here, consider this class
 * as just a C struct only with a lot of convenient constructors.
 */
class ItemGeometry final {
public:
  /// Constructs invalid geometry
  /// @see setRect(), setAdvance()
  constexpr ItemGeometry() noexcept = default;

  /// Constructs geometry based on rect @a r and advance values @a ax and @a ay
  constexpr ItemGeometry(const QRectF& r, qreal ax, qreal ay) noexcept
      : _rect(r), _a_sz(ax, ay) {}
  /**
   * @overload
   * First 4 arguments (@a x, @a y, @a w, @a h) are used to build the rect, @a ax and @a ay
   * are used to initialize corresponding advance values.
   */
  constexpr ItemGeometry(qreal x, qreal y, qreal w, qreal h, qreal ax, qreal ay) noexcept
      : _rect(x, y, w, h), _a_sz(ax, ay) {}
  /**
   * @overload
   * All arguments (@a x, @a y, @a w, @a h) are used to build rect, @a w and @a h
   * initialize horizontal and vertical advance correspondingly.
   */
  constexpr ItemGeometry(qreal x, qreal y, qreal w, qreal h) noexcept
      : ItemGeometry(x, y, w, h, w, h) {}
  /**
   * @overload
   * Creates geometry based on given rect @a r. Rect's width and height are used to
   * initialize horizontal and vertical advance correspondingly.
   */
  explicit constexpr ItemGeometry(const QRectF& r) noexcept
      : ItemGeometry(r, r.width(), r.height()) {}
  /**
   * @overload
   * Creates geometry based only on size @a sz. Sets rect's top-left to (0,0),
   * horizontal advance to @a sz.width(), vertical advance to @a sz.height()
   */
  explicit constexpr ItemGeometry(const QSizeF& sz) noexcept
      : ItemGeometry(0.0, 0.0, sz.width(), sz.height()) {}

  /// Bounding rect
  constexpr const QRectF& rect() const noexcept { return _rect; }

  /// Horizontal distance to the next item
  constexpr qreal advanceX() const noexcept { return _a_sz.width(); }
  /// Vertical distance to the next item
  constexpr qreal advanceY() const noexcept { return _a_sz.height(); }

  /// Changes rect to @a r, advance is not changed
  constexpr void setRect(const QRectF& r) noexcept { _rect = r; }
  /// @overload
  constexpr void setRect(const QRectF&& r) noexcept { _rect = std::move(r); }

  /// Changes horizontal advance to @a ax
  constexpr void setAdvanceX(qreal ax) noexcept { _a_sz.setWidth(ax); }
  /// Changes vertical advance to @a ay
  constexpr void setAdvanceY(qreal ay) noexcept { _a_sz.setHeight(ay); }
  /// Changes both advance values to @a ax and @a ay
  constexpr void setAdvance(qreal ax, qreal ay) noexcept { _a_sz = QSizeF(ax, ay); }

  friend constexpr inline bool operator==(const ItemGeometry& lhs, const ItemGeometry& rhs) noexcept
  {
    return lhs._rect == rhs._rect && lhs._a_sz == rhs._a_sz;
  }
  friend constexpr inline bool operator!=(const ItemGeometry& lhs, const ItemGeometry& rhs) noexcept
  {
    return !(lhs == rhs);
  }

private:
  QRectF _rect;
  QSizeF _a_sz;
};
