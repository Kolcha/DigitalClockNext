#include "application.hpp"
#include "application_private.hpp"

#include <ranges>
#include <utility>

void ApplicationPrivate::initWindows(QScreen* primary_screen, QList<QScreen*> screens)
{
  if (_app_config->global().getWindowPerScreen())
    std::ranges::for_each(std::as_const(screens), [this](auto s) { createWindow(s); });
  else
    createWindow(primary_screen);
}

std::size_t ApplicationPrivate::window_index(const ClockWindow* w) const noexcept
{
  for (std::size_t i = 0; i < _windows.size(); i++)
    if (_windows[i].get() == w)
      return i;
  return 0;   // fallback to the first window
}

void ApplicationPrivate::createWindow(const QScreen* screen)
{
  std::size_t idx = _app_config->global().getConfigPerWindow() ? _windows.size() : 0;
  const auto& cfg = _app_config->window(idx);
  // TODO: apply time zone, for now local time zone is hardcoded
  auto skin = idx == 0 && !_windows.empty() ? _windows.front()->skin() : _skin_manager->loadSkin(idx);
  auto wnd = std::make_unique<ClockWindow>(std::move(skin), _time_src->now().toLocalTime());
  connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::setDateTime);
  if (cfg.appearance().getFlashingSeparator())
    connect(_time_src.get(), &TimeSource::halfSecondUpdate, wnd.get(), &ClockWindow::setSeparatorVisible);
  wnd->show();
  _windows.emplace_back(std::move(wnd));
}

void Application::createWindows()
{
  _impl->initWindows(primaryScreen(), screens());
  for (const auto& wnd : _impl->windows()) {
    connect(wnd.get(), &ClockWindow::settingsDialogRequested, this, &Application::showSettingsDialog);
    connect(wnd.get(), &ClockWindow::aboutDialogRequested, this, &Application::showAboutDialog);
    connect(wnd.get(), &ClockWindow::appExitRequested, this, &Application::quit);
  }
}
