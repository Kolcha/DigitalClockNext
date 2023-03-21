#include "application.hpp"

#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

using namespace Qt::Literals::StringLiterals;

// constructor and destructor are required here to get
// std::unique_ptr to work with forward declarations
Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
{
}

Application::~Application() = default;

void Application::init()
{
  initTray();
}

// TODO: consider to move to own files
void Application::showSettingsDialog()
{
}

void Application::showAboutDialog()
{
}

void Application::initTray()
{
  createTrayIcon();
  createTrayMenu();
  _tray_icon->setContextMenu(_tray_menu.get());
  _tray_icon->setToolTip(u"Digital Clock Next"_s);
}

void Application::createTrayIcon()
{
  _tray_icon = std::make_unique<QSystemTrayIcon>();
  _tray_icon->setIcon(QIcon::fromTheme(u"clock"_s));
  _tray_icon->setVisible(true);
}

void Application::createTrayMenu()
{
  _tray_menu = std::make_unique<QMenu>();
  _tray_menu->addAction(QIcon::fromTheme(u"configure"_s), tr("&Settings"), this, &Application::showSettingsDialog);
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon::fromTheme(u"help-about"_s), tr("&About"), this, &Application::showAboutDialog);
  _tray_menu->addSeparator();
  _tray_menu->addAction(QIcon::fromTheme(u"application-exit"_s), tr("&Quit"), this, &QApplication::quit);
}
