#pragma once

#include <memory>

#include "render/layout_renderer.hpp"
#include "render/state_guard.hpp"
#include "skin/clock_skin.hpp"

// TODO: what about plugins' layouts? for now skin assumes only time
// TODO: handle plugins' layouts using Qt' stuff - widgets may be clickable
// TODO: consider shared renderer
// useless class, exists just for testing
// TODO: make it useful! it can be embedded into QWidget derivative
class ClockWidget final {
public:
  ClockWidget(const QDateTime& dt, std::shared_ptr<ClockSkin> skin)
    : _skin(skin)
    , _renderer(std::make_unique<LayoutRenderer>())
  {
    // initial value must be supplied to build layout
    setDateTime(dt);
  }

  void setDateTime(const QDateTime& dt)
  {
    _item = _skin->process(dt);
  }

  // TODO: what about fade in/out or glow animations?
  void setSeparatorVisible(bool visible)
  {
    renderable()->setSeparatorsVisible(visible);
  }

  bool isSeparatorVisible() const
  {
    return renderable()->areSeparatorsVisible();
  }

  void render(QPainter* p) const
  {
    if (!p) return;
    StateGuard _(p);
    p->translate(-_item->geometry().topLeft());
    _renderer->render(_item.get(), p);
  }

private:
  std::shared_ptr<ClockRenderable> renderable() const
  {
    return static_pointer_cast<ClockRenderable>(_item->renderable());
  }

private:
  std::shared_ptr<ClockSkin> _skin;
  std::unique_ptr<LayoutRenderer> _renderer;
  std::unique_ptr<RenderableItem> _item;
};
