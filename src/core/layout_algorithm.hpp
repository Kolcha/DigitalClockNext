#pragma once

#include <memory>
#include <vector>

#include "layout_item.hpp"

class LayoutItemModifier final {
public:
  void setParent(const std::shared_ptr<LayoutItem>& p) { _item.setParent(p); }
  void setTransform(QTransform t) { _item.setTransform(std::move(t)); }

private:
  friend class Layout;
  friend class LayoutAlgorithm;
  explicit LayoutItemModifier(LayoutItem& item) noexcept : _item(item) {}

private:
  LayoutItem& _item;
};


using LayoutItemsList = std::vector<std::shared_ptr<LayoutItem>>;


class LayoutAlgorithm {
public:
  virtual ~LayoutAlgorithm() = default;

  void operator ()(const LayoutItemsList& items) const { apply(items); }

protected:
  LayoutItemModifier modifier(LayoutItem& item) const noexcept
  {
    return LayoutItemModifier(item);
  }

private:
  virtual void apply(const LayoutItemsList& items) const = 0;
};
