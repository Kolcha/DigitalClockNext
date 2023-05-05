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

void ApplicationPrivate::createWindow(const QScreen* screen)
{
  std::size_t idx = _app_config->global().getConfigPerWindow() ? _windows.size() : 0;
  const auto& cfg = _app_config->window(idx);
  // TODO: apply time zone, for now local time zone is hardcoded
  auto skin = cfg.appearance().getUseFontInsteadOfSkin()
              ? _skin_manager->loadSkin(cfg.state().getTextSkinFont())
              : _skin_manager->loadSkin(cfg.state().getLastUsedSkin());
  _skin_manager->configureSkin(skin, idx);
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
}
