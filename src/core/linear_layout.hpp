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

#include "layout_algorithm.hpp"

/**
 * @brief Arranges items horizontally or vertically
 *
 * Layout algorithm that puts items in a row one after another optionally
 * adding some spacing between them.
 *
 * Spacing and orientation are customizable.
 *
 * @see LayoutItem::advanceX(), LayoutItem::advanceY(), setSpacing()
 */
class LinearLayout final : public LayoutAlgorithm {
public:
  /**
   * @brief Default constructor
   *
   * Provided for convenience. Constructs horizontal layout with no
   * additional spacing added between items.
   */
  LinearLayout() noexcept = default;

  /**
   * @brief Constructor
   *
   * Constructs layout with given orientation and spacing.
   *
   * @param o - layout's orientation
   * @param spacing - additional spacing
   */
  explicit LinearLayout(Qt::Orientation o, qreal spacing = 0.0) noexcept
    : _spacing(spacing)
  {
    setOrientation(o);
  }

  /// Current spacing
  qreal spacing() const noexcept { return _spacing; }
  /// Change @a spacing
  void setSpacing(qreal spacing) noexcept { _spacing = spacing; }

  /// Current orientation
  Qt::Orientation orientation() const noexcept
  {
    return _orientation == &vertical ? Qt::Vertical : Qt::Horizontal;
  }

  /// Change @a orientation
  void setOrientation(Qt::Orientation orientation) noexcept
  {
    _orientation = orientation == Qt::Vertical ? &vertical : &horizontal;
  }

  /// Is advance value ignored
  bool ignoreAdvance() const noexcept { return _ignore_advance; }
  /// Ignore advance value, use width or height instead
  void setIgnoreAdvance(bool ignore) noexcept { _ignore_advance = ignore; }

  /// Reimplements LayoutAlgorithm::apply()
  std::pair<qreal, qreal> apply(const ContainerType& items) const override;

private:
  struct OrientationImpl {
    QPointF(*position)(qreal);
    qreal(Glyph::*advance)() const;
    qreal(QPointF::*coord)() const;
    qreal(QRectF::*minCoord)() const;
    qreal(QRectF::*maxCoord)() const;
    void(QRectF::*setMinCoord)(qreal);
    void(QRectF::*setMaxCoord)(qreal);
  };

  static constexpr const OrientationImpl horizontal {
    [](qreal dx) { return QPointF(dx, 0); },
    &Glyph::advanceX,
    &QPointF::x,
    &QRectF::left,
    &QRectF::right,
    &QRectF::setLeft,
    &QRectF::setRight,
  };

  static constexpr const OrientationImpl vertical {
    [](qreal dy) { return QPointF(0, dy); },
    &Glyph::advanceY,
    &QPointF::y,
    &QRectF::top,
    &QRectF::bottom,
    &QRectF::setTop,
    &QRectF::setBottom,
  };

  const OrientationImpl* _orientation = &horizontal;
  qreal _spacing = 0.0;
  bool _ignore_advance = false;
};
