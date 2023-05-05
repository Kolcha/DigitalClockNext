#pragma once

#include "config_base_qvariant.hpp"

class AppearanceConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, UseFontInsteadOfSkin, true)
  CONFIG_OPTION_Q(bool, FlashingSeparator, true)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
