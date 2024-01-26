/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QString>
#include <QVariant>

#include "core/config_base.hpp"
#include "conversion_qvariant.hpp"

using ConfigBaseQVariant = ConfigBase<QString, QVariant>;

#define CONFIG_OPTION_Q(type, name, def_value) \
  CONFIG_OPTION(type, name, QLatin1String(#name), def_value)
