#pragma once

#include "config_base.hpp"

#include <QBrush>
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

// QBush seems not deserializes coordinate mode, and it is set
// to default (QGradient::LogicalMode) after deserialization
// this is not squitable, so re-create brush if required
template<>
struct fromValue<QVariant, QBrush> {
  QBrush operator ()(const QVariant& v)
  {
    QBrush brush = qvariant_cast<QBrush>(v);
    if (brush.gradient()) {
      QGradient gradient = *brush.gradient();
      gradient.setCoordinateMode(QGradient::ObjectMode);
      brush = QBrush(gradient);
    }
    return brush;
  }
};

// save QGradient as QBrush, Qt seems to have serializer for that
template<>
struct toValue<QGradient, QVariant> {
  QVariant operator ()(const QGradient& v)
  {
    return toValue<QBrush, QVariant>()(QBrush(v));
  }
};

template<>
struct fromValue<QVariant, QGradient> {
  QGradient operator ()(const QVariant& v)
  {
    return *fromValue<QVariant, QBrush>()(v).gradient();
  }
};

using ConfigBaseQVariant = ConfigBase<QString, QVariant>;

#define CONFIG_OPTION_Q(type, name, def_value) \
  CONFIG_OPTION(type, name, QLatin1String(#name), def_value)
