#pragma once

#include "app/clock_window.hpp"
#include "settings/fr_window_state.hpp"

#define BIND_WINDOW_STATE_PROPERTY(Type, Name)  \
  void set##Name(const Type& value) override    \
  { \
    _state.set##Name(value); _state.commit();   \
  } \
  Type get##Name() const override { return _state.get##Name(); }

class ClockWindowState final : public ClockWindow::State
{
public:
  explicit ClockWindowState(WindowState* state)
    : _state(*state)
  {}

  BIND_WINDOW_STATE_PROPERTY(QPoint, Pos)

private:
  WindowState& _state;
};
