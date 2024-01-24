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

#include <memory>

#include <QRect>

class QPainter;

// "skin resource"
class Resource {
public:
  virtual ~Resource() = default;

  virtual QRectF rect() const = 0;
  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  // no target rect is here,
  // item decides itself where to paint,
  // but this doesn't exceed its rect(),
  // painter must be configured instead
  virtual void draw(QPainter* p) = 0;

  virtual size_t cacheKey() const = 0;
};


class ResourceDecorator : public Resource {
public:
  explicit ResourceDecorator(std::shared_ptr<Resource> r)
    : _r(std::move(r))
  {}

  QRectF rect() const override { return _r->rect(); }
  qreal advanceX() const override { return _r->advanceX(); }
  qreal advanceY() const override { return _r->advanceY(); }

  void draw(QPainter* p) override { _r->draw(p); }

  size_t cacheKey() const override { return _r->cacheKey(); }

private:
  std::shared_ptr<Resource> _r;
};


class CachedResource : public ResourceDecorator {
public:
  using ResourceDecorator::ResourceDecorator;

  void draw(QPainter* p) override;
};


// resource just with given geometry,
// no draw() implementation is provided
class InvisibleResource final : public Resource {
public:
  InvisibleResource(QRectF r, qreal ax, qreal ay) noexcept
    : _rect(std::move(r))
    , _ax(ax)
    , _ay(ay)
  {}

  QRectF rect() const noexcept override { return _rect; }
  qreal advanceX() const noexcept override { return _ax; }
  qreal advanceY() const noexcept override { return _ay; }

  void draw(QPainter* p) noexcept override { Q_UNUSED(p); }

  size_t cacheKey() const noexcept override { return 0; }

private:
  QRectF _rect;
  qreal _ax;
  qreal _ay;
};
