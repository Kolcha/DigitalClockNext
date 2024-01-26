/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QString>
#include <QVariant>

#include "core/state_base.hpp"
#include "conversion_qvariant.hpp"

using StateBaseQVariant = StateBase<QString, QString, QVariant>;

#define STATE_OPTION_Q(type, name, def_value) \
  STATE_OPTION(type, name, QLatin1String(#name), def_value)
