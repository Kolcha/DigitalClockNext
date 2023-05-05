#pragma once

#include "config_base_qvariant.hpp"

class ClassicSkinConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(int, SpacingH, 0)
  CONFIG_OPTION_Q(int, SpacingV, 0)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
