/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023-2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "layout.hpp"

class LinearLayout : public Layout {
public:
  LinearLayout() noexcept = default;

  explicit LinearLayout(Qt::Orientation o, qreal spacing = 0.0) noexcept
    : _spacing(spacing)
  {
    setOrientation(o);
  }

  qreal spacing() const noexcept { return _spacing; }
  void setSpacing(qreal spacing) noexcept { _spacing = spacing; }

  Qt::Orientation orientation() const noexcept
  {
    return _orientation->current;
  }

  void setOrientation(Qt::Orientation orientation) noexcept
  {
    _orientation = orientation == Qt::Vertical ? &vertical : &horizontal;
  }

  bool ignoreAdvance() const noexcept { return _ignore_advance; }
  void setIgnoreAdvance(bool ignore) noexcept { _ignore_advance = ignore; }

  Qt::Alignment itemAlignment(std::size_t i) const noexcept
  {
    Q_ASSERT(0 <= i && i < _items_alignment.size());
    return _items_alignment[i];
  }

  void setItemAlignment(std::size_t i, Qt::Alignment a) noexcept
  {
    Q_ASSERT(0 <= i && i < _items_alignment.size());
    Q_ASSERT(_items_alignment.size() == _items.size());
    _items_alignment[i] = a;
  }

protected:
  void doAddItem(std::shared_ptr<LayoutItem> item) override
  {
    _items.push_back(std::move(item));
    _items_alignment.push_back(Qt::AlignBaseline | Qt::AlignJustify);
  }

  std::pair<qreal, qreal> doBuildLayout() override;

private:
  // items are resized only in the opposite direction
  // (e.g. in vertical direction for horizontal layout),
  // item itself decides how to handle resize
  // returns min/max coordinates (in opposite direction)
  std::pair<qreal, qreal> resizeItems();

  // applies alignment to the item
  // g is an item's geometry defined by layout
  void applyAlignment(LayoutItem& item, Qt::Alignment a, const QRectF& g);

  // constructs item's geometry using omin/omax
  // and applies alignment to the item at index i
  void applyAlignment(std::size_t i, qreal omin, qreal omax)
  {
    const auto& item = _items[i];
    auto g = _orientation->geometry(item->rect(), omin, omax);
    Q_ASSERT(_items_alignment.size() == _items.size());
    applyAlignment(*item, _items_alignment[i], g);
  }

  // set coordinate to 0 for opposite direction
  // required for correctly changing layout on run-time
  void resetPosInOppositeDirection();

private:
  struct Orientation {
    // just an orientation values
    Qt::Orientation current;
    Qt::Orientation opposite;

    // min/max coordinates in the same direction
    qreal(QRectF::*cmin)() const;
    qreal(QRectF::*cmax)() const;
    // min/max coordinates in opposite direction
    qreal(QRectF::*omin)() const;
    qreal(QRectF::*omax)() const;
    // r/w reference to coordinate in the same direction
    qreal&(QPointF::*cpos)();
    // r/w reference to coordinate in opposite direction
    qreal&(QPointF::*opos)();
    // calculates advance in the same direction
    qreal(*cadvance)(const LayoutItem& curr, const LayoutItem& prev);
    // calculates current item geometry based on min/max values
    QRectF(*geometry)(const QRectF& r, qreal omin, qreal omax);
    // calculates layout's advance values (ax, ay)
    using ItemsList = std::vector<std::shared_ptr<LayoutItem>>;
    std::pair<qreal, qreal>(*advances)(const ItemsList& items);
  };

  static const Orientation horizontal;
  static const Orientation vertical;

  std::vector<std::shared_ptr<LayoutItem>> _items;
  std::vector<Qt::Alignment> _items_alignment;
  qreal _spacing = 0;
  const Orientation* _orientation = &horizontal;
  bool _ignore_advance = false;
};
