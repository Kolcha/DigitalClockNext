/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <vector>

#include <QDateTime>
#include <QPoint>

#include "state_base_qvariant.hpp"

class AppState final {
  using BackendType = ConfigBackend<QString, QString, QVariant>;

public:
  class GlobalState final : public StateBaseQVariant {
    STATE_OPTION_Q(QDateTime, LastUpdateCheck, default_update_time())
  public:
    using StateBaseQVariant::StateBaseQVariant;
  private:
    static QDateTime default_update_time();
  };

  class WindowState final : public StateBaseQVariant {
    STATE_OPTION_Q(QPoint, Pos, default_pos())
    STATE_OPTION_Q(Qt::Alignment, Alignment, Qt::AlignTop | Qt::AlignLeft)
  public:
    WindowState(auto backend, std::size_t i)
      : StateBaseQVariant(std::move(backend), QString("State/Window%1").arg(i))
      , _index(i)
    {}
  private:
    inline QPoint default_pos() const noexcept
    {
      return QPoint(75, 50) + QPoint(25, 20) * static_cast<int>(_index);
    }
  private:
    std::size_t _index;
  };

  explicit AppState(std::shared_ptr<BackendType> backend)
    : _backend(std::move(backend))
    , _global(std::make_unique<GlobalState>(_backend, QLatin1String("State")))
  {}

  GlobalState& global() noexcept { return *_global; }

  WindowState& window(std::size_t i)
  {
    if (_wnd_states.size() < i + 1)
      _wnd_states.resize(i + 1);

    auto& wnd_state = _wnd_states[i];
    if (!wnd_state)
      wnd_state = std::make_unique<WindowState>(_backend, i);

    return *wnd_state;
  }

private:
  std::shared_ptr<BackendType> _backend;
  std::unique_ptr<GlobalState> _global;
  std::vector<std::unique_ptr<WindowState>> _wnd_states;
};
