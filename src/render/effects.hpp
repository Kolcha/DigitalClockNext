/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023-2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "resource.hpp"

#include <QBrush>

// creates new drawing surface and draws inner item on it
// new surface size is equal to the size of the orinal one
class NewSurfaceEffect final : public ResourceDecorator {
public:
  using ResourceDecorator::ResourceDecorator;

  void draw(QPainter* p) override;
};


// applies texture to inner item, tiles by default
class TexturingEffect final : public ResourceDecorator {
public:
  using ResourceDecorator::ResourceDecorator;

  void draw(QPainter* p) override;

  QBrush brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

  void setBrush(QBrush b) noexcept { _brush = std::move(b); }
  void setStretch(bool s) noexcept { _stretch = s; }

private:
  QBrush _brush = QColor(128, 64, 240);
  bool _stretch = false;
};


// fills background and draws inner item on it
class BackgroundEffect final : public ResourceDecorator {
public:
  using ResourceDecorator::ResourceDecorator;

  void draw(QPainter* p) override;

  QBrush brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

  void setBrush(QBrush b) noexcept { _brush = std::move(b); }
  void setStretch(bool s) noexcept { _stretch = s; }

private:
  QBrush _brush = QColor(240, 224, 64);
  bool _stretch = false;
};
