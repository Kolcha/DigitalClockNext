/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "layout_debug.hpp"

class DebugConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(debug::LayoutDebug, ItemDebugFlags, static_cast<debug::LayoutDebug>(0))
  CONFIG_OPTION_Q(debug::LayoutDebug, LayoutDebugFlags, static_cast<debug::LayoutDebug>(0))
  CONFIG_OPTION_Q(bool, EnableTopLevelDebug, false)
  CONFIG_OPTION_Q(bool, DisableCaching, false)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
