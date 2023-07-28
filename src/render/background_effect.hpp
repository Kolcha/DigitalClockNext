#pragma once

#include "core/effect.hpp"

#include <QPainter>

class BackgroundEffect final : public SimpleEffect {
public:
  explicit BackgroundEffect(QBrush brush = Qt::black) noexcept
    : _brush(std::move(brush))
  {}

  void setBrush(QBrush brush) noexcept
  {
    _brush = std::move(brush);
  }

  const QBrush& brush() const noexcept { return _brush; }

private:
  void simpleApply(QPainter* p, const QRectF& r) const override
  {
    // TODO: what about non-rectangular regions?
    p->fillRect(r, _brush);
  }

private:
  QBrush _brush;
};
