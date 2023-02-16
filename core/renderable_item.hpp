#pragma once

#include "layout_item.hpp"

#include <assert.h>
#include <memory>
#include <utility>
#include <vector>

#include "effect.hpp"
#include "renderable.hpp"

class RenderableItem final : public LayoutItem {
  using RenderablePtr = std::shared_ptr<Renderable>;
  using Effects = std::vector<std::shared_ptr<Effect>>;

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

  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

  const Effects& effects() const noexcept { return _effects; }

private:
  RenderablePtr _renderable;
  Effects _effects;
};
