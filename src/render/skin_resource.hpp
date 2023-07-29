#pragma once

#include "core/hashable.hpp"

#include <QPainter>
#include <QRectF>

class SkinResource : public Hashable {
public:
  virtual ~SkinResource() = default;

  virtual QRectF rect() const = 0;

  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  virtual void render(QPainter* p) = 0;
};
