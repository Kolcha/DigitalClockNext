#pragma once

#include "render/effect.hpp"

// does nothing, just renders already visible content
// may be required to apply layout effect on top of item effects
class IdentityEffect final : public Effect {
public:
  IdentityEffect() noexcept { updateHash(0); }
};
