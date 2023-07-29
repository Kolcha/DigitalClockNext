#pragma once

#include "settings/config_base_qvariant.hpp"

class AppearanceConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, UseFontInsteadOfSkin, true)
  CONFIG_OPTION_Q(bool, FlashingSeparator, true)
  CONFIG_OPTION_Q(int, ScaleFactorX, 100)
  CONFIG_OPTION_Q(int, ScaleFactorY, 100)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
