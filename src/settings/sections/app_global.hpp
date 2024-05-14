/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config_base_qvariant.hpp"

class AppGlobalConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(int, WindowsCount, 1)
  CONFIG_OPTION_Q(bool, ConfigPerWindow, false)
  CONFIG_OPTION_Q(bool, StayOnTop, false)
  CONFIG_OPTION_Q(bool, TransparentForMouse, false)
  CONFIG_OPTION_Q(bool, CheckForUpdates, true)
  CONFIG_OPTION_Q(bool, CheckForBetaVersion, true)
  CONFIG_OPTION_Q(int, UpdatePeriodDays, 7)
  CONFIG_OPTION_Q(bool, SnapToEdge, true)
  CONFIG_OPTION_Q(int, SnapThreshold, 10)
  CONFIG_OPTION_Q(bool, ChangeOpacityOnMouseHover, false)
  CONFIG_OPTION_Q(qreal, OpacityOnMouseHover, 0.1)
  CONFIG_OPTION_Q(bool, EnableDebugOptions, false)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
