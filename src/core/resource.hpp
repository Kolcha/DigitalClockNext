/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
