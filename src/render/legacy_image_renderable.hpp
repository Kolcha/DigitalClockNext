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

#include "skin_resource.hpp"

#include <memory>

#include <QIcon>
#include <QPainter>
#include <QSvgRenderer>

#include "glyph_geometry.hpp"

class LegacyImageRenderable : public SkinResource {
public:
  explicit LegacyImageRenderable(const QString& filename)
    : m_hash(qHash(filename))
  {}

  std::size_t hash() const noexcept override { return m_hash; }

  QRectF rect() const override { return m_gg.rect(); }

  qreal advanceX() const override { return m_gg.advanceX(); }
  qreal advanceY() const override { return m_gg.advanceY(); }

protected:
  void setGeometry(GlyphGeometry gg) noexcept { m_gg = std::move(gg); }

private:
  GlyphGeometry m_gg;
  std::size_t m_hash;   // do not calculate hash every time
};


class RasterImageRenderable : public LegacyImageRenderable
{
public:
  RasterImageRenderable(const QString& filename, const GlyphGeometryRaw& gargs)
    : LegacyImageRenderable(filename)
    , m_icon(filename)
    , m_size(m_icon.availableSizes().constFirst())
  {
    setGeometry({gargs, m_size});
  }

  void render(QPainter* p) override
  {
    p->drawPixmap(rect().toRect(), m_icon.pixmap(m_size));
  }

private:
  QIcon m_icon;   // QIcon perfectly handles HighDPI
  QSize m_size;   // 100% image size
};


class SvgImageRenderable : public LegacyImageRenderable
{
public:
  SvgImageRenderable(const QString& filename, const GlyphGeometryRaw& gargs)
    : LegacyImageRenderable(filename)
    , m_renderer(std::make_unique<QSvgRenderer>(filename))
  {
    setGeometry({gargs, m_renderer->viewBoxF().size()});
  }

  void render(QPainter* p) override
  {
    m_renderer->render(p, rect());
  }

private:
  std::unique_ptr<QSvgRenderer> m_renderer;
};
