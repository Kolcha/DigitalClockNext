#pragma once

#include "core/effect.hpp"

#include <QPainter>
#include <QPixmap>

#include "core/renderable.hpp"

class TexturingEffect : public Effect {
public:
  void setBrush(QBrush brush)
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
//      tx_painter.translate(-item->rect().topLeft());
      r(item, &tx_painter);
      tx_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      tx_painter.setPen(Qt::NoPen);
      tx_painter.setBrush(_brush);
      tx_painter.drawRect(QRectF(QPointF(0, 0), buffer.deviceIndependentSize()));
    }
    p->drawPixmap(0, 0, buffer);
  }

private:
  QBrush _brush = Qt::magenta;
};
