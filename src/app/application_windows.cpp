/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "application.hpp"
#include "application_private.hpp"

#include <algorithm>
#include <utility>

#include <QGraphicsEffect>
#include <QPixmapCache>

#include "window_state.hpp"

void ApplicationPrivate::initWindows(QScreen* primary_screen, QList<QScreen*> screens)
{
  int windows_count = std::clamp(_app_config->global().getWindowsCount(), 1 , 8);
  for (int i = 0; i < windows_count; i++) createWindow(nullptr);
  std::ranges::for_each(_windows, [this](auto&& wnd) { configureWindow(wnd.get()); });
}

#ifdef Q_OS_WINDOWS
void ApplicationPrivate::initStayOnTopHacks()
{
  if (!_app_config->global().getStayOnTop()) return;
  _win_stay_on_top_hacks = std::make_unique<WinStayOnToHacks>();
  connect(_time_src.get(), &TimeSource::timeChanged, _win_stay_on_top_hacks.get(), &WinStayOnToHacks::apply);
  std::ranges::for_each(_windows, [this](auto&& wnd) { _win_stay_on_top_hacks->addWindow(wnd.get()); });
}
#endif

void ApplicationPrivate::configureWindow(ClockWindow* wnd)
{
  const std::size_t widx = window_index(wnd);
  std::size_t idx = _app_config->global().getConfigPerWindow() ? window_index(wnd) : 0;
  const auto& cfg = _app_config->window(idx);
  auto skin = idx != widx ? window(idx)->skin() : _skin_manager->loadSkin(idx);
  wnd->setSkin(std::move(skin));
  wnd->setSnapToEdge(_app_config->global().getSnapToEdge());
  wnd->setSnapThreshold(_app_config->global().getSnapThreshold());
  wnd->changeOpacityOnMouseHover(_app_config->global().getChangeOpacityOnMouseHover());
  wnd->setOpacityOnMouseHover(_app_config->global().getOpacityOnMouseHover());
  if (!_app_config->window(widx).general().getShowLocalTime())
    wnd->setTimeZone(_app_config->window(widx).state().getTimeZone());
  wnd->setWindowOpacity(cfg.appearance().getOpacity());
  wnd->setSeparatorFlashes(cfg.appearance().getFlashingSeparator());
  wnd->scale(cfg.appearance().getScaleFactorX(), cfg.appearance().getScaleFactorY());
  if (cfg.appearance().getApplyColorization()) {
    auto effect = new QGraphicsColorizeEffect;
    effect->setColor(cfg.appearance().getColorizationColor());
    effect->setStrength(cfg.appearance().getColorizationStrength());
    wnd->setGraphicsEffect(effect);
  } else {
    // explicitly set effect to nullptr as this code is used for config reload
    wnd->setGraphicsEffect(nullptr);
  }
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
  auto state = std::make_unique<ClockWindowState>(&_app_state->window(_windows.size()));
  auto wnd = std::make_unique<ClockWindow>(std::move(state));
  wnd->setDateTime(_time_src->now().toLocalTime());
  if (_app_config->global().getStayOnTop()) {
    wnd->setWindowFlag(Qt::WindowStaysOnTopHint);
    wnd->setWindowFlag(Qt::BypassWindowManagerHint);
  }
  if (_app_config->global().getTransparentForMouse())
    wnd->setWindowFlag(Qt::WindowTransparentForInput);
#ifdef Q_OS_WINDOWS
  wnd->setWindowFlag(Qt::Tool);   // trick to hide app icon from taskbar (Windows only)
#endif
  connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::setDateTime);
  if (_windows.empty() || _app_config->global().getConfigPerWindow())
    connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::animateSeparator);
  if (_mouse_tracker && _app_config->global().getChangeOpacityOnMouseHover())
    connect(_mouse_tracker.get(), &MouseTracker::mousePositionChanged, wnd.get(), &ClockWindow::handleMouseMove);
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
  std::ranges::for_each(_impl->windows(), [](auto&& wnd) { wnd->show(); });
#ifdef Q_OS_WINDOWS
  _impl->initStayOnTopHacks();
#endif
}
