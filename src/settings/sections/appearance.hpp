/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config_base_qvariant.hpp"

#include <QColor>

class AppearanceConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, UseFontInsteadOfSkin, false)
  CONFIG_OPTION_Q(bool, FlashingSeparator, true)
  CONFIG_OPTION_Q(bool, ApplyColorization, false)
  CONFIG_OPTION_Q(int, ScaleFactorX, 100)
  CONFIG_OPTION_Q(int, ScaleFactorY, 100)
  CONFIG_OPTION_Q(qreal, Opacity, 0.85)
  CONFIG_OPTION_Q(qreal, ColorizationStrength, 0.8)
  CONFIG_OPTION_Q(QColor, ColorizationColor, QColor(0, 85, 255))
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
