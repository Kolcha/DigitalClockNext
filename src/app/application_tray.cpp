#include "application.hpp"
#include "application_private.hpp"

#include "clock_tray_icon.hpp"

using namespace Qt::Literals::StringLiterals;

void ApplicationPrivate::initTray()
{
  _tray_icon = std::make_unique<ClockTrayIcon>();
  _tray_icon->setVisible(true);

  _tray_menu = std::make_unique<QMenu>();
  _tray_icon->setContextMenu(_tray_menu.get());
}

void Application::initTray()
{
  _impl->initTray();

  auto cfg_action = _impl->tray_menu()->addAction(
      QIcon::fromTheme(u"configure"_s), tr("Settings"),
      this, &Application::showSettingsDialog);
  const auto& cfg = _impl->app_config()->global();
  cfg_action->setDisabled(cfg.getWindowsCount() > 1);
  _impl->tray_menu()->addSeparator();
  _impl->tray_menu()->addAction(QIcon::fromTheme(u"help-about"_s), tr("About"),
                                this, &Application::showAboutDialog);
  _impl->tray_menu()->addSeparator();
  _impl->tray_menu()->addAction(QIcon::fromTheme(u"application-exit"_s),
                                tr("Quit"), this, &QApplication::quit);

  _impl->tray_icon()->setToolTip(applicationDisplayName());
}
