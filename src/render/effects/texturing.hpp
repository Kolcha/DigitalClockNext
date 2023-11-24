#pragma once

#include "render/effect.hpp"

class TexturingEffect final : public SimpleEffect {
public:
  explicit TexturingEffect(QBrush brush = Qt::magenta, bool stretch = false);

  void setBrush(QBrush brush);
  void setStretch(bool stretch);

  const QBrush& brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

private:
  void simpleApply(QPainter* p, const QRectF& r) const override;

private:
  QBrush _brush;
  bool _stretch;
};
