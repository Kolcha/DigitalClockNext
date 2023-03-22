#pragma once

#include <memory>
#include <utility>

#include "settings.hpp"

#define CONFIG_OPTION(type, name, key, def_value) \
  private:            \
    type _##name;     \
  public:             \
    void set##name(const type& val) { _client->setValue(key, val); } \
    type get##name() const { return _client->value(key, def_value); }


template<typename Key, typename Value>
class ConfigBase {
  using ConfigClientType = ConfigClient<Key, Value>;

public:
  explicit ConfigBase(std::unique_ptr<ConfigClientType> client) noexcept
    : _client(std::move(client))
  {}

private:
  std::unique_ptr<ConfigClientType> _client;
};
