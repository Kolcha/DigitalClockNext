#pragma once

#include "config_base_qvariant.hpp"

#include <QString>

using namespace Qt::Literals::StringLiterals;

class AppearanceConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION(QString, Skin, u"skin"_s, u"text"_s)
  CONFIG_OPTION(bool, FlashingSeparator, u"flashing_separator"_s, true)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
