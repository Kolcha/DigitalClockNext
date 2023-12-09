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

#include <QColor>
#include <QGradient>
#include <QFont>
#include <QPixmap>
#include <QPoint>
#include <QString>
#include <QTimeZone>

class WindowState final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QFont, TextSkinFont, default_font())
  CONFIG_OPTION_Q(QString, LastUsedSkin, QString("Electronic (built-in)"))
  CONFIG_OPTION_Q(QColor, TextureColor, QColor(112, 96, 240))
  CONFIG_OPTION_Q(QGradient, TextureGradient, sample_conical_gradient())
  CONFIG_OPTION_Q(QPixmap, TexturePattern, sample_pattern())
  CONFIG_OPTION_Q(QColor, BackgroundColor, QColor(0, 0, 0, 160))
  CONFIG_OPTION_Q(QGradient, BackgroundGradient, sample_linear_gradient())
  CONFIG_OPTION_Q(QPixmap, BackgroundPattern, sample_pattern())
  CONFIG_OPTION_Q(QTimeZone, TimeZone, QTimeZone::systemTimeZone())
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
private:
  static QFont default_font();
  static QGradient sample_conical_gradient();
  static QGradient sample_linear_gradient();
  static QPixmap sample_pattern();
};
