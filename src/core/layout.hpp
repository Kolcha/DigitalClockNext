#pragma once

#include "layout_item.hpp"

#include <algorithm>
#include <memory>

#include "layout_algorithm.hpp"

class Layout : public LayoutItem {
public:
  void addItem(std::shared_ptr<LayoutItem> item)
  {
    if (!item) return;

    LayoutItemModifier(*item).setParent(shared_from_this());
    _items.push_back(std::move(item));

    updateGeometry();
  }

  void setAlgorithm(std::shared_ptr<LayoutAlgorithm> algo)
  {
    if (!algo) return;

    _algorithm = std::move(algo);

    updateGeometry();
  }

private:
  void updateGeometry() override
  {
    QRectF r;
    if (_algorithm && !_items.empty()) {
      (*_algorithm)(_items);
      // TODO: std::transform_reduce ?
      r = std::accumulate(
            std::next(_items.begin()), _items.end(),
            _items.front()->geometry(),
            [](const auto& r, const auto& i) { return r | i->geometry(); }
      );
    }
    // TODO: what about dx/dy? protected members ?
    setRect(std::move(r));
  }

private:
  std::shared_ptr<LayoutAlgorithm> _algorithm;
  LayoutItemsList _items;
};
