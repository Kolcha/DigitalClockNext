#pragma once

#include "config_base.hpp"

#include <QString>
#include <QVariant>

using ConfigBaseQVariant = ConfigBase<QString, QVariant>;

#define CONFIG_OPTION_Q(type, name, def_value) \
  CONFIG_OPTION(type, name, QLatin1String(#name), def_value)
