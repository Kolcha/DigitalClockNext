#pragma once

#include "core/renderable.hpp"

#include <memory>

#include "render/composite_effect.hpp"

class RenderableBase : public Renderable {
public:
  RenderableBase()
    : _effect(std::make_unique<CompositeEffect>())
  {}

  void render(QPainter* p) const final
  {
    if (!_visible || !p)
      return;

    _effect->apply(this, p, [this](const Renderable* r, QPainter* p) { renderImpl(p); });
  }

  bool isVisible() const final { return _visible; }
  void setVisible(bool visible) final { _visible = visible; }

  void addEffect(std::shared_ptr<Effect> effect) final
  {
    _effect->addEffect(std::move(effect));
  }

protected:
  virtual void renderImpl(QPainter* p) const = 0;

private:
  bool _visible = true;
  std::unique_ptr<CompositeEffect> _effect;
};
