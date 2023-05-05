#include "application.hpp"
#include "application_private.hpp"

void ApplicationPrivate::initWindows()
{
  // TODO: create multiple windows, for now only one is supported
  // TODO: apply time zone, for now local time zone is hardcoded
  const auto& appearance = _app_config->window(0).appearance();
  const auto& state = _app_config->window(0).state();
  auto skin = appearance.getUseFontInsteadOfSkin()
              ? _skin_manager->loadSkin(state.getTextSkinFont())
              : _skin_manager->loadSkin(state.getLastUsedSkin());
  _skin_manager->configureSkin(skin, 0);
  auto wnd = std::make_unique<ClockWindow>(std::move(skin), _time_src->now().toLocalTime());
  connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::setDateTime);
  if (appearance.getFlashingSeparator())
    connect(_time_src.get(), &TimeSource::halfSecondUpdate, wnd.get(), &ClockWindow::setSeparatorVisible);
  wnd->show();
  _windows.emplace_back(std::move(wnd));
}

void Application::createWindows()
{
  _impl->initWindows();
}
