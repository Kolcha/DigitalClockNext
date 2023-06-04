#pragma once

#include "core/effect.hpp"

// does nothing, just renders already visible content
// may be required to apply layout effect on top of item effects
class IdentityEffect final : public Effect {
public:
  void apply(const Renderable* item, QPainter* p, RenderItemFn r) const override
  {
    r(item, p);
  }
};
