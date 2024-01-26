/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config_base_qvariant.hpp"

#include "layout_debug.hpp"

class DebugConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(debug::LayoutDebug, ItemDebugFlags, static_cast<debug::LayoutDebug>(0))
  CONFIG_OPTION_Q(debug::LayoutDebug, LayoutDebugFlags, static_cast<debug::LayoutDebug>(0))
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
