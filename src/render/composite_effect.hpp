#pragma once

#include "core/effect.hpp"

#include <memory>
#include <ranges>
#include <vector>

#include "core/renderable.hpp"

class CompositeEffect final : public Effect {
public:
  void apply(const Renderable* item, QPainter* p, RenderItemFn r) const override
  {
    if (_effects.empty() || item->rect().size().isEmpty()) {
      r(item, p);
    } else {
      std::ranges::for_each(_effects, [=](const auto& e) { e->apply(item, p, r); });
    }
  }

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
