#pragma once

#include "config_base_qvariant.hpp"

class GeneralConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, ShowLocalTime, true)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
