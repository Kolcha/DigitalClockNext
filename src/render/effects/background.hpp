#pragma once

#include "render/effect.hpp"

class BackgroundEffect final : public SimpleEffect {
public:
  explicit BackgroundEffect(QBrush brush = Qt::black);

  void setBrush(QBrush brush);

  const QBrush& brush() const noexcept { return _brush; }

private:
  void simpleApply(QPainter* p, const QRectF& r) const override
  {
    p->fillRect(r, _brush);
  }

private:
  QBrush _brush;
};
