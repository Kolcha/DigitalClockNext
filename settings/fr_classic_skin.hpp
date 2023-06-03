#pragma once

#include "config_base_qvariant.hpp"

#include <QString>

class ClassicSkinConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QString, TimeFormat, QString("hh:mm:ss a"))
  CONFIG_OPTION_Q(Qt::Orientation, Orientation, Qt::Horizontal)
  CONFIG_OPTION_Q(int, Spacing, 0)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
