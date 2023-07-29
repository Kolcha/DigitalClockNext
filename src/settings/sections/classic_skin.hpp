#pragma once

#include "settings/config_base_qvariant.hpp"

#include <QBrush>
#include <QString>

class ClassicSkinConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QString, TimeFormat, QString("hh:mm:ss a"))
  CONFIG_OPTION_Q(Qt::Orientation, Orientation, Qt::Horizontal)
  CONFIG_OPTION_Q(int, Spacing, 0)
  CONFIG_OPTION_Q(QBrush, Texture, QBrush(Qt::NoBrush))
  CONFIG_OPTION_Q(QBrush, Background, QBrush(Qt::NoBrush))
  CONFIG_OPTION_Q(bool, TexturePerElement, true)
  CONFIG_OPTION_Q(bool, BackgroundPerElement, false)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
