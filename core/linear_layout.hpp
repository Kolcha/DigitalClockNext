#pragma once

#include "layout.hpp"

#include <algorithm>
#include <utility>

class LinearLayout : public Layout {
public:
  LinearLayout(Qt::Orientation o, qreal spacing = 0.)
    : Layout()
    , _spacing(spacing)
  {
    setOrientation(o);
  }

  qreal spacing() const noexcept { return _spacing; }

  void setSpacing(qreal spacing)
  {
    if (qFuzzyCompare(spacing, _spacing))
      return;

    _spacing = spacing;

    updateGeometry();
  }

  Qt::Orientation orientation() const noexcept
  {
    return _orientation == &vertical ? Qt::Vertical : Qt::Horizontal;
  }

  void setOrientation(Qt::Orientation orientation)
  {
    if (orientation == this->orientation())
      return;

    _orientation = orientation == Qt::Vertical ? &vertical : &horizontal;

    updateGeometry();
  }

  void updateGeometry() final
  {
    auto&& items = this->items();

    if (items.empty()) {
      this->setRect({});
      return;
    }

    std::ranges::for_each(items, [](auto& i) { i->setPos({}); });

    auto g = items.front()->geometry();
    qreal dpos = 0;
    for (const auto& item : items) {
      auto pos = item->pos();
      (pos.*_orientation->wpos)(dpos);
      item->setPos(std::move(pos));
      dpos += (*item.*_orientation->advance)() + _spacing;
      g |= item->geometry();
    }

    this->setRect(std::move(g));
    Layout::updateGeometry();
  }

private:
  struct OrientationImpl {
    void(QPointF::*wpos)(qreal);
    qreal(LayoutItem::*advance)() const;
  };

  static constexpr const OrientationImpl horizontal {
    &QPointF::setX,
    &LayoutItem::advanceX
  };

  static constexpr const OrientationImpl vertical {
    &QPointF::setY,
    &LayoutItem::advanceY
  };

  const OrientationImpl* _orientation = &horizontal;
  qreal _spacing;
};
