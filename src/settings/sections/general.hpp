#pragma once

#include "settings/config_base_qvariant.hpp"

class GeneralConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, ShowLocalTime, true)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
