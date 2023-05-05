#pragma once

#include "config_base.hpp"

#include <QString>
#include <QVariant>

template<typename T>
struct toValue<T, QVariant> {
  QVariant operator ()(const T& v) { return QVariant::fromValue(v); }
};

template<typename T>
struct fromValue<QVariant, T> {
  T operator ()(const QVariant& v) { return qvariant_cast<T>(v); }
};

using ConfigBaseQVariant = ConfigBase<QString, QVariant>;

#define CONFIG_OPTION_Q(type, name, def_value) \
  CONFIG_OPTION(type, name, QLatin1String(#name), def_value)
