#pragma once

#include "config_base_qvariant.hpp"

#include <QFont>
#include <QString>

class WindowState final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(QFont, TextSkinFont, QFont("Segoe Script", 48))
  CONFIG_OPTION_Q(QString, LastUsedSkin, QString())
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
