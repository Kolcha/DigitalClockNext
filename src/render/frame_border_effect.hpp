#pragma once

#include "core/effect.hpp"

class FrameBorderEffect final : public SimpleEffect {
public:
  explicit FrameBorderEffect(QPen p = QColor(85, 85, 255));

  void setPen(QPen p);
  const QPen& pen() const noexcept { return _pen; }

private:
  void simpleApply(QPainter* p, const QRectF& r) const override;

private:
  QPen _pen;
};
