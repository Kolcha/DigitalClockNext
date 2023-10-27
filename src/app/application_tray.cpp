#include "application.hpp"
#include "application_private.hpp"

using namespace Qt::Literals::StringLiterals;

void ApplicationPrivate::initTray()
{
  QIcon icon = QIcon::fromTheme(u"clock"_s);
  icon.setIsMask(true);
  _tray_icon = std::make_unique<QSystemTrayIcon>(icon);
  _tray_icon->setVisible(true);

  _tray_menu = std::make_unique<QMenu>();
  _tray_icon->setContextMenu(_tray_menu.get());

  auto on_msg_clicked = [this]() { disconnect(_tray_icon.get(), &QSystemTrayIcon::messageClicked, nullptr, nullptr); };
  connect(_tray_icon.get(), &QSystemTrayIcon::messageClicked, this, on_msg_clicked);
}

void Application::initTray()
{
  _impl->initTray();

  _impl->tray_menu()->addAction(QIcon::fromTheme(u"configure"_s), tr("&Settings"),
                                this, &Application::showSettingsDialog);
  _impl->tray_menu()->addSeparator();
  _impl->tray_menu()->addAction(QIcon::fromTheme(u"help-about"_s), tr("&About"),
                                this, &Application::showAboutDialog);
  _impl->tray_menu()->addSeparator();
  _impl->tray_menu()->addAction(QIcon::fromTheme(u"application-exit"_s),
                                tr("&Quit"), this, &QApplication::quit);

  _impl->tray_icon()->setToolTip(applicationDisplayName());
}
