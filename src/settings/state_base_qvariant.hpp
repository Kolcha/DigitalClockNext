#pragma once

#include <QString>
#include <QVariant>

#include "settings/core/state_base.hpp"
#include "settings/conversion_qvariant.hpp"

using StateBaseQVariant = StateBase<QString, QString, QVariant>;

#define STATE_OPTION_Q(type, name, def_value) \
  STATE_OPTION(type, name, QLatin1String(#name), def_value)
