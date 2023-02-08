#pragma once

#include <QRectF>

class Renderable {
public:
  virtual ~Renderable() = default;

  // TODO: maybe rename to geometry
  // means "original bounding rect", i.e. as was returned from the source
  // this value will be used by layout as base value for transormation
  virtual QRectF rect() const = 0;

  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  // no target rect here!
  // it is renderer responsibility to setup coordinate system
  virtual void render() const = 0;

  // TODO: consider `noexcept` just because the only flag is assumed
  virtual bool isVisible() const = 0;
  virtual void setVisible(bool visible) = 0;
};


class QPainter;
// used by implementation-specific LayoutRenderer
// LayoutRenderer does cast to this interface
class RenderableBase : public Renderable {
public:
  // called before every render() call
  // TODO: maybe rename to updateContext()
  // TODO: consider to make it protected (and LayoutRenderer as friend)
  void setRenderContext(QPainter* p) { _p = p; }

  void render() const final
  {
    if (!_visible || !_p)
      return;

    renderImpl(_p);
  }

  bool isVisible() const final { return _visible; }
  void setVisible(bool visible) final { _visible = visible; }

protected:
  virtual void renderImpl(QPainter* p) const = 0;

private:
  bool _visible = true;
  QPainter* _p = nullptr;
};
