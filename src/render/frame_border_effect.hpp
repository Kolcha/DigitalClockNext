#pragma once

#include "core/effect.hpp"

#include <QPen>

class FrameBorderEffect final : public SimpleEffect {
public:
  explicit FrameBorderEffect(QPen p = QColor(85, 85, 255))
    : _pen(std::move(p))
  {}

  void setPen(QPen p) { _pen = std::move(p); }

private:
  void simpleApply(QPainter* p, const QRectF& r) const override
  {
    p->save();
    p->setPen(_pen);
    p->setBrush(Qt::NoBrush);
    p->drawRect(r);
    p->restore();
  }

private:
  QPen _pen;
};
