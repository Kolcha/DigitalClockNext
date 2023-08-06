#pragma once

#include "settings/config_base_qvariant.hpp"

class AppGlobalConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, WindowPerScreen, false)
  CONFIG_OPTION_Q(bool, ConfigPerWindow, false)
  CONFIG_OPTION_Q(bool, StayOnTop, false)
  CONFIG_OPTION_Q(bool, TransparentForMouse, false)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
