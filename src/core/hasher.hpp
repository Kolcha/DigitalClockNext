#pragma once

#include <functional>
#include <memory>
#include <numeric>
#include <ranges>
#include <type_traits>

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

#include "hashable.hpp"

// there is no overload for size_t in QDataStream ...
// so use macro tricks to cast it to appropriate quint
inline QDataStream& operator<<(QDataStream& s, size_t v)
{
#define CC_IMPL(a, b) a##b
#define CC(a, b) CC_IMPL(a, b)
  s << static_cast<CC(quint, __SIZE_WIDTH__)>(v);
#undef CC_IMPL
#undef CC
  return s;
}

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

// hashes a container of shared_ptr hashable objects
template <std::ranges::input_range R>
requires std::is_convertible_v<std::ranges::range_value_t<R>, std::shared_ptr<Hashable>>
size_t hasher(R&& r)
{
  auto has_invalid_hash = [](auto&& item) { return item->hash() == Hashable::Invalid; };
  if (std::ranges::any_of(r, has_invalid_hash))
    return Hashable::Invalid;

  return std::transform_reduce(
        std::ranges::cbegin(r), std::ranges::cend(r),
        static_cast<size_t>(0),
        std::bit_xor{},
        [](const auto& item) { return item->hash(); }
  );
}
