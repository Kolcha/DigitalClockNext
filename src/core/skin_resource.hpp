#pragma once

#include <QPainter>
#include <QRectF>

// TODO: forward declare QPainter

class SkinResource {
public:
  virtual ~SkinResource() = default;

  virtual QRectF rect() const = 0;

  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  virtual void render(QPainter* p) = 0;
};
