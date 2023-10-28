#pragma once

#include <QString>
#include <QVariant>

#include "settings/core/config_base.hpp"
#include "settings/conversion_qvariant.hpp"

using ConfigBaseQVariant = ConfigBase<QString, QVariant>;

#define CONFIG_OPTION_Q(type, name, def_value) \
  CONFIG_OPTION(type, name, QLatin1String(#name), def_value)
