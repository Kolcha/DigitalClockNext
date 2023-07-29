#pragma once

#include "render/effect.hpp"

class TexturingEffect final : public SimpleEffect {
public:
  explicit TexturingEffect(QBrush brush = Qt::magenta);

  void setBrush(QBrush brush);

  const QBrush& brush() const noexcept { return _brush; }

private:
  void simpleApply(QPainter* p, const QRectF& r) const override;

private:
  QBrush _brush;
};
