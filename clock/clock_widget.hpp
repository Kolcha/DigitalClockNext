#pragma once

#include <memory>

#include "skin/clock_skin.hpp"
#include "render/state_guard.hpp"

// TODO: what about plugins' layouts? for now skin assumes only time
// TODO: handle plugins' layouts using Qt' stuff - widgets may be clickable
// useless class, exists just for testing
class ClockWidget final {
public:
  ClockWidget(QStringView str, std::shared_ptr<ClockSkin> skin)
    : _skin(skin)
  {
    // initial value must be supplied to build layout
    setDateTime(str);
  }

  // TODO: replace with date/time type
  // ignored for now, just use hardcoded string
  void setDateTime(QStringView str)
  {
    _rendarable = _skin->process(str);
  }

  // TODO: what about fade in/out or glow animations?
  void setSeparatorVisible(bool visible)
  {
    _rendarable->setSeparatorsVisible(visible);
  }

  bool isSeparatorVisible() const { return _rendarable->areSeparatorsVisible(); }

  void render(QPainter* p) const
  {
    if (!p) return;
    StateGuard _(p);
    _rendarable->setRenderContext(p);
    _rendarable->render();
  }

private:
  std::shared_ptr<ClockSkin> _skin;
  std::shared_ptr<ClockRenderable> _rendarable;
};
