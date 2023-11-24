#pragma once

#include "settings/config_base_qvariant.hpp"

#include <QBrush>
#include <QString>

class ClassicSkinConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QString, TimeFormat, default_time_format())
  CONFIG_OPTION_Q(Qt::Orientation, Orientation, Qt::Horizontal)
  CONFIG_OPTION_Q(int, Spacing, 8)
  CONFIG_OPTION_Q(QBrush, Texture, QBrush(QColor(112, 96, 240)))
  CONFIG_OPTION_Q(QBrush, Background, QBrush(Qt::NoBrush))
  CONFIG_OPTION_Q(bool, TexturePerElement, true)
  CONFIG_OPTION_Q(bool, BackgroundPerElement, false)
  CONFIG_OPTION_Q(QString, CustomSeparators, QString(":"))
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
private:
  static QString default_time_format();
};
