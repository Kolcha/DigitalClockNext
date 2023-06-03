#pragma once

#include "core/effect.hpp"

#include <QPainter>
#include <QPixmap>

class TexturingEffect : public Effect {
public:
  explicit TexturingEffect(QBrush brush = Qt::magenta) noexcept
    : _brush(std::move(brush))
  {}

  void setBrush(QBrush brush) noexcept
  {
    _brush = std::move(brush);
  }

  void apply(const Renderable* item, QPainter* p, RenderItemFn r) const override
  {
    QPixmap buffer(p->device()->width(), p->device()->height());
    buffer.setDevicePixelRatio(p->device()->devicePixelRatioF());
    buffer.fill(Qt::transparent);
    {
      QPainter tx_painter(&buffer);
      r(item, &tx_painter);
      tx_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      tx_painter.setPen(Qt::NoPen);
      tx_painter.setBrush(_brush);
      tx_painter.drawRect(QRectF(QPointF(0, 0), buffer.deviceIndependentSize()));
    }
    p->drawPixmap(0, 0, buffer);
  }

private:
  QBrush _brush;
};
