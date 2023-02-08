#pragma once

#include "layout_item.hpp"

#include <assert.h>
#include <memory>
#include <utility>

#include "renderable.hpp"

class RenderableItem final : public LayoutItem {
  using RenderablePtr = std::shared_ptr<Renderable>;

public:
  explicit RenderableItem(RenderablePtr renderable)
  {
    setRenderable(std::move(renderable));
  }

  void setRenderable(RenderablePtr renderable)
  {
    assert(renderable);
    _renderable = std::move(renderable);
    this->setRect(_renderable->rect());
  }

  auto renderable() const noexcept { return _renderable; }

  qreal advanceX() const override { return _renderable->advanceX(); }
  qreal advanceY() const override { return _renderable->advanceY(); }

private:
  RenderablePtr _renderable;
};
