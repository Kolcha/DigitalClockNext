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
  // TODO: move lines below to some state object
  CONFIG_OPTION_Q(QDateTime, LastUpdateCheck, QDateTime(QDate(2013, 6, 18), QTime(11, 20, 5), QTimeZone::utc()))
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
