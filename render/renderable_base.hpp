#pragma once

#include "core/renderable.hpp"

class RenderableBase : public Renderable {
public:
  void render(QPainter* p) const final
  {
    if (!_visible || !p)
      return;

    renderImpl(p);
  }

  bool isVisible() const final { return _visible; }
  void setVisible(bool visible) final { _visible = visible; }

protected:
  virtual void renderImpl(QPainter* p) const = 0;

private:
  bool _visible = true;
};
