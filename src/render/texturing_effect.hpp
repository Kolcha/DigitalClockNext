#pragma once

#include "core/effect.hpp"

#include <QPainter>

#include "render/state_guard_qpainter.hpp"

class TexturingEffect final : public SimpleEffect {
public:
  explicit TexturingEffect(QBrush brush = Qt::magenta) noexcept
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
    StateGuard _(p);
    p->setCompositionMode(QPainter::CompositionMode_SourceIn);
    p->setPen(Qt::NoPen);
    p->setBrush(_brush);
    p->drawRect(r.adjusted(-0.51, -0.51, 0.51, 0.51));  // ?
  }

private:
  QBrush _brush;
};
