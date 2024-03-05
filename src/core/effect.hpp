/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "resource.hpp"

// acts as factory for resource decorators
// keeps effect's configuration and
// configures decorator according to it
class Effect {
public:
  virtual ~Effect() = default;

  using ResourcePtr = std::shared_ptr<Resource>;
  virtual ResourcePtr decorate(ResourcePtr res) = 0;
};
