/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

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

#include <memory>

#include <QIcon>
#include <QSvgRenderer>

#include "geometry.hpp"

class ImageResource : public Resource {
public:
  explicit ImageResource(const QString& filename) noexcept
    : m_hash(qHash(filename))
  {}

  QRectF rect() const noexcept final { return m_gg.rect(); }
  qreal advanceX() const noexcept final { return m_gg.advanceX(); }
  qreal advanceY() const noexcept final { return m_gg.advanceY(); }

  size_t cacheKey() const noexcept final { return m_hash; }

  void setGeometry(const QRectF& r, qreal ax, qreal ay) noexcept
  {
    setGeometry(Geometry(r, ax, ay));
  }
  void setGeometry(Geometry gg) noexcept { m_gg = std::move(gg); }

private:
  Geometry m_gg;
  size_t m_hash;  // do not calculate hash every time
};


class RasterImageResource : public ImageResource {
public:
  explicit RasterImageResource(const QString& filename)
    : ImageResource(filename)
    , m_icon(filename)
    , m_size(m_icon.availableSizes().constFirst())
  {
    setGeometry(Geometry(m_size));
  }

  void draw(QPainter* p) override;

private:
  QIcon m_icon;   // QIcon perfectly handles HighDPI
  QSize m_size;   // 100% image size
};


class SvgImageResource : public ImageResource {
public:
  explicit SvgImageResource(const QString& filename)
    : ImageResource(filename)
    , m_renderer(std::make_unique<QSvgRenderer>(filename))
  {
    setGeometry(Geometry(m_renderer->defaultSize()));
  }

  void draw(QPainter* p) override;

private:
  std::unique_ptr<QSvgRenderer> m_renderer;
};
