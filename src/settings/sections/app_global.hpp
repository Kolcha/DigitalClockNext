#pragma once

#include "settings/config_base_qvariant.hpp"

class AppGlobalConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, WindowPerScreen, false)
  CONFIG_OPTION_Q(bool, ConfigPerWindow, false)
  CONFIG_OPTION_Q(bool, StayOnTop, false)
  CONFIG_OPTION_Q(bool, TransparentForMouse, false)
  CONFIG_OPTION_Q(bool, CheckForUpdates, true)
  CONFIG_OPTION_Q(bool, CheckForBetaVersion, false)
  CONFIG_OPTION_Q(int, UpdatePeriodDays, 7)
  CONFIG_OPTION_Q(bool, SnapToEdge, true)
  CONFIG_OPTION_Q(int, SnapThreshold, 10)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
