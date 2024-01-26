/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "resource_factory.hpp"

#include <QFont>

class FontResource final : public Resource {
public:
  FontResource(const QFont& font, char32_t ch);

  QRectF rect() const noexcept override { return _br; }
  qreal advanceX() const noexcept override { return _ax; }
  qreal advanceY() const noexcept override { return _ay; }

  void draw(QPainter* p) override;

  size_t cacheKey() const noexcept override { return _hash; }

private:
  char32_t _ch;
  QRectF _br;
  qreal _ax;
  qreal _ay;
  const QFont& _font;
  size_t _hash;
};


class FontResourceFactory final : public ResourceFactory {
public:
  explicit FontResourceFactory(QFont font) noexcept
      : _font(std::move(font))
  {}

public:
  const QFont& font() const noexcept { return _font; }

  qreal ascent() const override;
  qreal descent() const override;

protected:
  std::shared_ptr<Resource> create(char32_t ch) const override
  {
    return std::make_shared<FontResource>(_font, ch);
  }

private:
  QFont _font;
};
