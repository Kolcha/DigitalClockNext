/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "app/clock_window.hpp"
#include "app_state.hpp"

#define BIND_WINDOW_STATE_PROPERTY(Type, Name)  \
  void set##Name(const Type& value) override    \
  { \
    _state.set##Name(value);  \
  } \
  Type get##Name() const override { return _state.get##Name(); }

class ClockWindowState final : public ClockWindow::State
{
public:
  explicit ClockWindowState(AppState::WindowState* state)
    : _state(*state)
  {}

  BIND_WINDOW_STATE_PROPERTY(QPoint, Pos)
  BIND_WINDOW_STATE_PROPERTY(Qt::Alignment, Alignment)

private:
  AppState::WindowState& _state;
};
