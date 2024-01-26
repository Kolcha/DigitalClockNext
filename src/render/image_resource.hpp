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

class ImageResource : public Resource {
public:
  explicit ImageResource(const QString& filename) noexcept
    : m_hash(qHash(filename))
  {}

  QRectF rect() const noexcept final { return m_rect; }
  qreal advanceX() const noexcept final { return m_ax; }
  qreal advanceY() const noexcept final { return m_ay; }

  size_t cacheKey() const noexcept final { return m_hash; }

  void setGeometry(QRectF r, qreal ax, qreal ay) noexcept
  {
    m_rect = std::move(r);
    m_ax = ax;
    m_ay = ay;
  }

protected:
  void initGeometry(const QSizeF& sz) noexcept
  {
    m_rect = QRectF({0.0, 0.0}, sz);
    m_ax = sz.width();
    m_ay = sz.height();
  }

private:
  QRectF m_rect;
  qreal m_ax = 0.0;
  qreal m_ay = 0.0;
  size_t m_hash;  // do not calculate hash every time
};


class RasterImageResource : public ImageResource {
public:
  explicit RasterImageResource(const QString& filename)
    : ImageResource(filename)
    , m_icon(filename)
    , m_size(m_icon.availableSizes().constFirst())
  {
    initGeometry(m_size);
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
    initGeometry(m_renderer->defaultSize());
  }

  void draw(QPainter* p) override;

private:
  std::unique_ptr<QSvgRenderer> m_renderer;
};
