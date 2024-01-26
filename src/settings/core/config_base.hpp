/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <utility>

#include "settings.hpp"

#define CONFIG_OPTION(type, name, key, def_value) \
  public:             \
    void set##name(const type& val) { client().setValue(key, val); } \
    type get##name() const { return client().value(key, def_value); }


template<typename Key, typename Value>
class ConfigBase {
  using ConfigClientType = ConfigClient<Key, Value>;

public:
  explicit ConfigBase(std::unique_ptr<ConfigClientType> client) noexcept
    : _client(std::move(client))
  {}

  inline void commit() { _client->commit(); }
  inline void discard() { _client->discard(); }

protected:
  inline ConfigClientType& client() const noexcept { return *_client; }

private:
  std::unique_ptr<ConfigClientType> _client;
};
