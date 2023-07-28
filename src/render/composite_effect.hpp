#pragma once

#include "core/effect.hpp"

#include <memory>
#include <ranges>
#include <vector>

class CompositeEffect final : public Effect {
  void apply(QPainter* p, const QRectF& rc, RenderItemFn r) const override
  {
    if (_effects.empty() || rc.size().isEmpty()) {
      r(p);
    } else {
      std::ranges::for_each(_effects, [=](const auto& e) { (*e)(p, rc, r); });
    }
  }

public:
  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

  void clearEffects() noexcept
  {
    _effects.clear();
  }

  const auto& effects() const noexcept { return _effects; }

private:
  std::vector<std::shared_ptr<Effect>> _effects;
};
