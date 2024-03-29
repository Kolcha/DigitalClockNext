/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QBrush>
#include <QString>
#include <QTimeZone>
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
// this is not suitable, so re-create brush if required
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

// serialize QTimeZone as QString (IANA ID)
template<>
struct toValue<QTimeZone, QVariant> {
  QVariant operator ()(const QTimeZone& v)
  {
    return toValue<QString, QVariant>()(QString::fromLatin1(v.id()));
  }
};

template<>
struct fromValue<QVariant, QTimeZone> {
  QTimeZone operator ()(const QVariant& v)
  {
    return QTimeZone(fromValue<QVariant, QString>()(v).toLatin1());
  }
};

// serialize QFlags<> as int
template<typename T>
struct toValue<QFlags<T>, QVariant> {
  QVariant operator ()(const QFlags<T>& v)
  {
    return toValue<int, QVariant>()(static_cast<int>(v));
  }
};

template<typename T>
struct fromValue<QVariant, QFlags<T>> {
  QFlags<T> operator ()(const QVariant& v)
  {
    return static_cast<QFlags<T>>(fromValue<QVariant, int>()(v));
  }
};
