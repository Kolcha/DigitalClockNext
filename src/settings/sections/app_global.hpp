/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "config_base_qvariant.hpp"

class AppGlobalConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(int, WindowsCount, 1)
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
