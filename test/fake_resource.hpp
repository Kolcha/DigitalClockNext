#pragma once

#include "resource.hpp"

class FakeResource final : public Resource {
public:
  FakeResource(QRectF r, qreal ax, qreal ay) noexcept
      : _rect(std::move(r))
      , _ax(ax)
      , _ay(ay)
  {}

  QRectF rect() const noexcept override { return _rect; }
  qreal advanceX() const noexcept override { return _ax; }
  qreal advanceY() const noexcept override { return _ay; }

  void draw([[maybe_unused]] QPainter* p) override {}

  size_t cacheKey() const noexcept override { return 0; }

private:
  QRectF _rect;
  qreal _ax;
  qreal _ay;
};
