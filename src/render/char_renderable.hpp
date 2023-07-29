#pragma once

#include "core/skin_resource.hpp"

#include <QFont>

class QCharRenderable final : public SkinResource {
public:
  QCharRenderable(QChar ch, const QFont& font);

  size_t hash() const noexcept override { return _hash; }

  QRectF rect() const override { return _br; }

  qreal advanceX() const override { return _ax; }
  qreal advanceY() const override { return _ay; }

  void render(QPainter* p) override;

private:
  QChar _ch;
  QRectF _br;
  qreal _ax;
  qreal _ay;
  const QFont& _font;
  size_t _hash;
};
