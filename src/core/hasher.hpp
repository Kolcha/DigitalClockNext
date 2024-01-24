#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

// this hasher may be pretty heavy but yet flexible
// as so as it uses serialization under the hood
// use with caution, avoid recalculation if possible
template<typename... T>
size_t hasher(T&&... objs)
{
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  (stream << ... << objs);
  return qHash(buffer);
}
