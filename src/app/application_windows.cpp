#include "application.hpp"
#include "application_private.hpp"

#include <algorithm>
#include <utility>

#include <QPixmapCache>

#include "window_state.hpp"

void ApplicationPrivate::initWindows(QScreen* primary_screen, QList<QScreen*> screens)
{
  // TODO: what about multiple windows on the same screen, e.g. per time zone?
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
  auto skin = idx == 0 && !_windows.empty() ? _windows.front()->skin() : _skin_manager->loadSkin(idx);
  auto state = std::make_unique<ClockWindowState>(&_app_state->window(idx));
  auto wnd = std::make_unique<ClockWindow>(std::move(skin), _time_src->now().toLocalTime(), std::move(state));
  if (_app_config->global().getStayOnTop()) {
    wnd->setWindowFlag(Qt::WindowStaysOnTopHint);
    wnd->setWindowFlag(Qt::BypassWindowManagerHint);
  }
  if (_app_config->global().getTransparentForMouse())
    wnd->setWindowFlag(Qt::WindowTransparentForInput);
#ifndef Q_OS_MACOS
  wnd->setWindowFlag(Qt::Tool);   // trick to hide app icon from taskbar (Win/Linux)
#endif
  if (!cfg.general().getShowLocalTime())
    wnd->setTimeZone(cfg.state().getTimeZone());
  wnd->setWindowOpacity(cfg.appearance().getOpacity());
  wnd->setSeparatorFlashes(cfg.appearance().getFlashingSeparator());
  wnd->scale(cfg.appearance().getScaleFactorX(), cfg.appearance().getScaleFactorY());
  connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::setDateTime);
  connect(_time_src.get(), &TimeSource::halfSecondUpdate, wnd.get(), &ClockWindow::flipSeparator);
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
  // TODO: change pixmap cache size depending on scaling
  // for "common" (because cache is shared) 16 MB + 16 MB per window
  QPixmapCache::setCacheLimit((1 + _impl->windows().size()) * 16 * 1024);
}
