#pragma once

#include "layout.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <utility>

class LinearLayout : public Layout {
public:
  LinearLayout(Qt::Orientation o, qreal spacing = 0.)
    : Layout()
    , _spacing(spacing)
  {
    setOrientation(o);
  }
/*
  void addItem(std::unique_ptr<LayoutItem> item, bool rebuild = true)
  {
    Layout::addItem(std::move(item));
    if (rebuild)
      rebuildLayout();
  }
*/
  qreal spacing() const noexcept { return _spacing; }

  void setSpacing(qreal spacing)
  {
    if (qFuzzyCompare(spacing, _spacing))
      return;

    _spacing = spacing;

    rebuildLayout();
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

    rebuildLayout();
  }

protected:
  void updateGeometry() final
  {
    auto&& items = this->items();
    if (items.empty()) {
      this->setRect({});
    } else {
      auto g = std::accumulate(items.begin(), items.end(),
                               items.front()->geometry(),
                               [](const auto& a, const auto& i) { return a | i->geometry(); });
      this->setRect(std::move(g));
    }
  }

  void rebuildLayout()
  {
    auto&& items = this->items();

    if (items.empty())
      return;

    std::ranges::for_each(items, [](auto& i) { i->setGeometry(i->rect()); });

    auto prev_item = items.front().get();
    for (auto iter = std::next(items.begin()); iter != items.end(); ++iter) {
      auto curr_item = iter->get();
      auto ppos = (prev_item->geometry().*_orientation->rpos)();
      auto pd00 = (prev_item->rect().*_orientation->rpos)();
      auto padv = (prev_item->*_orientation->advance)();
      QTransform t;
      (_orientation->translate)(t, ppos - pd00 + padv + _spacing);
      curr_item->setTransform(std::move(t));
      prev_item = curr_item;
    }
  }

private:
  struct OrientationImpl {
    qreal(QRectF::*rpos)() const;
    void(*translate)(QTransform&, qreal);
    qreal(QRectF::*length)() const;
    qreal(LayoutItem::*advance)() const;
  };

  static constexpr const OrientationImpl horizontal {
    &QRectF::x,
    [](QTransform& tr, qreal dx) { tr.translate(dx, 0); },
    &QRectF::width,
    &LayoutItem::advanceX
  };

  static constexpr const OrientationImpl vertical {
    &QRectF::y,
    [](QTransform& tr, qreal dy) { tr.translate(0, dy); },
    &QRectF::height,
    &LayoutItem::advanceY
  };

  const OrientationImpl* _orientation = &horizontal;
  qreal _spacing;
};
