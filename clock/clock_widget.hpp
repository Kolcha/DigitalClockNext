#pragma once

#include <memory>

#include "render/layout_renderer.hpp"
#include "render/state_guard.hpp"
#include "skin/clock_skin.hpp"

// TODO: what about plugins' layouts? for now skin assumes only time
// TODO: handle plugins' layouts using Qt' stuff - widgets may be clickable
// useless class, exists just for testing
class ClockWidget final {
public:
  ClockWidget(QStringView str, std::shared_ptr<ClockSkin> skin)
    : _skin(skin)
    , _renderer(std::make_unique<LayoutRenderer>())
  {
    // initial value must be supplied to build layout
    setDateTime(str);
  }

  // TODO: replace with date/time type
  // ignored for now, just use hardcoded string
  void setDateTime(QStringView str)
  {
    _item = _skin->process(str);
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
    p->translate(-_item->rect().topLeft());
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
