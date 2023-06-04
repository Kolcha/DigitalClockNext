#pragma once

#include "layout_item.hpp"

#include <assert.h>
#include <memory>
#include <utility>
#include <vector>

#include <QTransform>

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
    applyTransform();
  }

  auto renderable() const noexcept { return _renderable; }

  // TODO: make dependant on transform
  qreal advanceX() const override { return _renderable->advanceX(); }
  qreal advanceY() const override { return _renderable->advanceY(); }

  void setTransform(QTransform transform)
  {
    if (transform == _transform)
      return;

    _transform = std::move(transform);
    applyTransform();
  }

  const QTransform& transform() const noexcept { return _transform; }

  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

  const Effects& effects() const noexcept { return _effects; }

private:
  void applyTransform()
  {
    assert(_renderable);
    setRect(_transform.mapRect(_renderable->rect()));
    updateGeometry();
  }

private:
  RenderablePtr _renderable;
  QTransform _transform;
  Effects _effects;
};
