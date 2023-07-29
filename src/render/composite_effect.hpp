#pragma once

#include "core/effect.hpp"

#include <memory>
#include <vector>

class CompositeEffect final : public Effect {
public:
  void addEffect(std::shared_ptr<Effect> effect);
  void clearEffects();

  const auto& effects() const noexcept { return _effects; }

private:
  void apply(QPainter* p, const QRectF& r,
             RenderItemFn render_item) const override;

private:
  std::vector<std::shared_ptr<Effect>> _effects;
};
