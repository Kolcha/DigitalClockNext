#pragma once

#include "layout_item.hpp"

#include <assert.h>

#include <memory>
#include <utility>
#include <vector>

class Layout : public LayoutItem {
  using ItemsList = std::vector<std::unique_ptr<LayoutItem>>;

public:
  const ItemsList& items() const noexcept { return _items; }

protected:
  void addItem(std::unique_ptr<LayoutItem> item)
  {
    assert(item);
    item->_parent = this;
    _items.push_back(std::move(item));
  }

  void clearItems() noexcept { _items.clear(); }

private:
  ItemsList _items;
};
