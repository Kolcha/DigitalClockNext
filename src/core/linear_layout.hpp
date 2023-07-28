#pragma once

#include "layout_algorithm.hpp"

class LinearLayout final : public LayoutAlgorithm {
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
    return _orientation == &vertical ? Qt::Vertical : Qt::Horizontal;
  }

  void setOrientation(Qt::Orientation orientation) noexcept
  {
    _orientation = orientation == Qt::Vertical ? &vertical : &horizontal;
  }

private:
  void apply(const LayoutItemsList& items) const override
  {
    qreal dpos = 0;
    for (const auto& item : items) {
      modifier(*item).setTransform((*_orientation->transform)(dpos));
      dpos += (*item.*_orientation->advance)() + _spacing;
    }
  }

private:
  struct OrientationImpl {
    QTransform(*transform)(qreal);
    qreal(LayoutItem::*advance)() const;
  };

  static constexpr const OrientationImpl horizontal {
    [](qreal dx) { return QTransform::fromTranslate(dx, 0); },
    &LayoutItem::advanceX
  };

  static constexpr const OrientationImpl vertical {
    [](qreal dy) { return QTransform::fromTranslate(0, dy); },
    &LayoutItem::advanceY
  };

  const OrientationImpl* _orientation = &horizontal;
  qreal _spacing = 0.0;
};
