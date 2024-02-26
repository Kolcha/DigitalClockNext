/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "config_base_qvariant.hpp"

#include <QBrush>
#include <QString>

class ClassicSkinConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QString, TimeFormat, default_time_format())
  CONFIG_OPTION_Q(Qt::Orientation, Orientation, Qt::Horizontal)
  CONFIG_OPTION_Q(int, Spacing, 8)
  CONFIG_OPTION_Q(QBrush, Texture, QBrush(QColor(112, 96, 240)))
  CONFIG_OPTION_Q(QBrush, Background, QBrush(Qt::NoBrush))
  CONFIG_OPTION_Q(bool, TextureStretch, false)
  CONFIG_OPTION_Q(bool, BackgroundStretch, false)
  CONFIG_OPTION_Q(bool, TexturePerElement, true)
  CONFIG_OPTION_Q(bool, BackgroundPerElement, false)
  CONFIG_OPTION_Q(QString, CustomSeparators, QString(":"))
  CONFIG_OPTION_Q(bool, IgnoreAdvanceX, false)
  CONFIG_OPTION_Q(bool, IgnoreAdvanceY, true)
  CONFIG_OPTION_Q(int, GlyphBaseHeight, 100)
  CONFIG_OPTION_Q(QString, LayoutConfig, QString("0"))
  CONFIG_OPTION_Q(int, SecondsScaleFactor, 100);
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
private:
  static QString default_time_format();
};
