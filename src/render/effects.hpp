/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "effect.hpp"
#include "resource.hpp"

#include <QBrush>

// creates new drawing surface and draws inner item on it
// new surface size is equal to the size of the orinal one
class NewSurfaceDecorator final : public ResourceDecorator {
public:
  using ResourceDecorator::ResourceDecorator;

  void draw(QPainter* p) override;
};

class NewSurfaceEffect final : public Effect {
public:
  ResourcePtr decorate(ResourcePtr res) override;
};

// applies texture to inner item, tiles by default
class TexturingDecorator final : public ResourceDecorator {
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

class TexturingEffect final : public Effect {
public:
  ResourcePtr decorate(ResourcePtr res) override;

  QBrush brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

  void setBrush(QBrush b) noexcept { _brush = std::move(b); }
  void setStretch(bool s) noexcept { _stretch = s; }

private:
  QBrush _brush = QColor(128, 64, 240);
  bool _stretch = false;
};

// fills background and draws inner item on it
class BackgroundDecorator final : public ResourceDecorator {
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

class BackgroundEffect final : public Effect {
public:
  ResourcePtr decorate(ResourcePtr res) override;

  QBrush brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

  void setBrush(QBrush b) noexcept { _brush = std::move(b); }
  void setStretch(bool s) noexcept { _stretch = s; }

private:
  QBrush _brush = QColor(240, 224, 64);
  bool _stretch = false;
};
