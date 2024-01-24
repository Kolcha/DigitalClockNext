/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2024  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
