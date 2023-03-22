#pragma once

#include "config_base_qvariant.hpp"

#include <QString>

using namespace Qt::Literals::StringLiterals;

class ClassicSkinConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION(int, SpacingH, u"spacing_h"_s, 0)
  CONFIG_OPTION(int, SpacingV, u"spacing_v"_s, 0)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
