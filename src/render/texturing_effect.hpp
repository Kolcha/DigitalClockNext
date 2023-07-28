#pragma once

#include "core/effect.hpp"

#include <QPainter>
#include <QPixmap>

#include "render/state_guard_qpainter.hpp"

class TexturingEffect : public Effect {
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
  void apply(QPainter* p, const QRectF& rc, RenderItemFn r) const override
  {
    StateGuard _(p);
    QPixmap buffer(p->device()->width(), p->device()->height());
    buffer.setDevicePixelRatio(p->device()->devicePixelRatioF());
    buffer.fill(Qt::transparent);
    {
      QPainter tx_painter(&buffer);
      tx_painter.setTransform(p->transform());
      r(&tx_painter);
      tx_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      tx_painter.setPen(Qt::NoPen);
      tx_painter.setBrush(_brush);
      tx_painter.drawRect(rc.adjusted(-0.51, -0.51, 0.51, 0.51));
    }
    p->resetTransform();
    p->drawPixmap(0, 0, buffer);
  }

private:
  QBrush _brush;
};
