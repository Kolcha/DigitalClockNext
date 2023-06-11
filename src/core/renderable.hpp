#pragma once

#include <QRectF>

class QPainter;

class Renderable {
public:
  virtual ~Renderable() = default;

  // means "original bounding rect", i.e. as was returned from the source
  // this value will be used by layout as base value for transormation
  virtual QRectF rect() const = 0;

  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  // no target rect here!
  // it is renderer responsibility to setup coordinate system
  virtual void render(QPainter* p) const = 0;

  virtual bool isVisible() const = 0;
  virtual void setVisible(bool visible) = 0;
};
