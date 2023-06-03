#pragma once

#include "core/effect.hpp"

#include <QPainter>

class BackgroundEffect : public Effect {
public:
  explicit BackgroundEffect(QBrush brush = Qt::black) noexcept
    : _brush(std::move(brush))
  {}

  void setBrush(QBrush brush) noexcept
  {
    _brush = std::move(brush);
  }

  void apply(const Renderable* item, QPainter* p, RenderItemFn r) const override
  {
    Q_UNUSED(item);
    Q_UNUSED(r);
    // TODO: what about non-rectangular regions?
    p->fillRect(QRect(0, 0, p->device()->width(), p->device()->height()), _brush);
  }

private:
  QBrush _brush;
};
