#pragma once

#include "settings/config_base_qvariant.hpp"

class GeneralConfig final : public ConfigBaseQVariant {
  CONFIG_OPTION_Q(bool, ShowLocalTime, true)
  CONFIG_OPTION_Q(Qt::Alignment, Alignment, Qt::AlignTop | Qt::AlignLeft)
public:
  using ConfigBaseQVariant::ConfigBaseQVariant;
};
