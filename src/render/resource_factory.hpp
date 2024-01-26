/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QHash>

#include "resource.hpp"

class ResourceFactory {
public:
  virtual ~ResourceFactory() = default;

  std::shared_ptr<Resource> item(char32_t ch) const
  {
    auto& resource = _cache[ch];
    if (!resource) resource = create(ch);
    return resource;
  }

  virtual qreal ascent() const = 0;
  virtual qreal descent() const = 0;

  inline qreal height() const { return ascent() + descent(); }

protected:
  virtual std::shared_ptr<Resource> create(char32_t ch) const = 0;

private:
  mutable QHash<char32_t, std::shared_ptr<Resource>> _cache;
};
