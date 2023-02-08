#pragma once

#include <memory>
#include <ranges>
#include <vector>

#include "skin/clock_skin.hpp"
#include "render/layout.hpp"
#include "render/state_guard.hpp"

// TODO: what about plugins' layouts? for now skin assumes only time
// TODO: handle plugins' layouts using Qt' stuff - widgets may be clickable
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
    _layout = _skin->layout(str);
    // TODO: separators should be const - not!
    // TODO: but what about separator or format change?
    // TODO: skin should allow to subscribe to event which require layout rebuild
    _seps = _skin->separators();
  }

  // TODO: what about fade in/out or glow animations?
  void setSeparatorVisible(bool visible)
  {
    _sep_visible = visible;
    std::ranges::for_each(_seps, [=](auto s) { s->setVisible(visible); });
  }

  bool isSeparatorVisible() const noexcept { return _sep_visible; }

  void render(QPainter* p) const
  {
    if (!p) return;
    StateGuard _(p);
    render_item(_layout.get(), p);
  }

private:
  std::shared_ptr<ClockSkin> _skin;
  std::unique_ptr<Layout> _layout;
  std::vector<std::shared_ptr<Renderable>> _seps;
  bool _sep_visible = true;
};
