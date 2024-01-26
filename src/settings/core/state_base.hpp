/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <utility>

#include "settings.hpp"

#define STATE_OPTION(type, name, key, def_value) \
  public:             \
    void set##name(const type& val) { setValue(key, val); } \
    type get##name() const { return value(key, def_value); }


template<typename Tag, typename Key, typename Value>
class StateBase {
  using BackendType = ConfigBackend<Tag, Key, Value>;

public:
  StateBase(std::shared_ptr<BackendType> backend, Tag tag)
    : _backend(std::move(backend))
    , _tag(std::move(tag))
  {
    _backend->load(_tag);
  }

  StateBase(const StateBase&) = delete;
  StateBase(StateBase&&) = default;

  StateBase& operator=(const StateBase&) = delete;
  StateBase& operator=(StateBase&&) = default;

protected:
  template<typename T>
  void setValue(const Key& k, const T& v)
  {
    _backend->setValue(_tag, k, toValue<T, Value>{}(v));
    _backend->save(_tag);
  }

  template<typename T>
  T value(const Key& k, const T& def) const
  {
    auto val = _backend->value(_tag, k);
    return val ? fromValue<Value, T>{}(*val) : def;
  }

private:
  std::shared_ptr<BackendType> _backend;
  Tag _tag;
};
