/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config_base_qvariant.hpp"

class GeneralConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, ShowLocalTime, true)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
