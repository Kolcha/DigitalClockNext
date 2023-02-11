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
    QSizeF sz = item->rect().size() * p->device()->devicePixelRatioF();
    if (sz.isEmpty()) return;                           // TODO: make first?
    QPixmap buffer(sz.toSize());
    buffer.setDevicePixelRatio(p->device()->devicePixelRatioF());
    buffer.fill(Qt::transparent);
    {
      QPainter tx_painter(&buffer);
      tx_painter.translate(-item->rect().topLeft());
      r(item, &tx_painter);
      tx_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      tx_painter.setPen(Qt::NoPen);
      tx_painter.setBrush(_brush);
      tx_painter.drawRect(item->rect());
    }
    p->drawPixmap(item->rect(), buffer, buffer.rect());
  }

private:
  QBrush _brush = Qt::magenta;
};
