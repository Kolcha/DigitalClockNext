#pragma once

#include <memory>

#include "core/rendering.hpp"
#include "render/state_guard_qpainter.hpp"
#include "skin/clock_skin.hpp"

// TODO: what about plugins' layouts? for now skin assumes only time
// TODO: handle plugins' layouts using Qt' stuff - widgets may be clickable
// TODO: consider shared renderer
class ClockWidget final {
public:
  ClockWidget(const QDateTime& dt, std::shared_ptr<ClockSkin> skin)
    : _skin(skin)
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
    _item->setSeparatorsVisible(visible);
  }

  bool isSeparatorVisible() const
  {
    return _item->areSeparatorsVisible();
  }

  void render(QPainter* p) const
  {
    if (!p) return;
    StateGuard _(p);
    p->translate(-_item->geometry().topLeft());
    _item->render(p);
  }

  QRectF geometry() const
  {
    return _item->geometry();
  }

private:
  std::shared_ptr<ClockSkin> _skin;
  std::shared_ptr<ClockRenderable> _item;
};
