#pragma once

#include <memory>
#include <vector>

#include <QDateTime>
#include <QPoint>

#include "settings/state_base_qvariant.hpp"

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
    STATE_OPTION_Q(QPoint, Pos, QPoint(75, 50))
    STATE_OPTION_Q(Qt::Alignment, Alignment, Qt::AlignTop | Qt::AlignLeft)
  public:
    using StateBaseQVariant::StateBaseQVariant;
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
      wnd_state = std::make_unique<WindowState>(_backend, QString("State/Window%1").arg(i));

    return *wnd_state;
  }

private:
  std::shared_ptr<BackendType> _backend;
  std::unique_ptr<GlobalState> _global;
  std::vector<std::unique_ptr<WindowState>> _wnd_states;
};
