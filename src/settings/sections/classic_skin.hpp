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
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
private:
  static QString default_time_format();
};
