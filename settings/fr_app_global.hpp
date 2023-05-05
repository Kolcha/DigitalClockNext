#pragma once

#include "config_base_qvariant.hpp"

class AppGlobalConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, WindowPerScreen, false)
  CONFIG_OPTION_Q(bool, ConfigPerWindow, false)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
