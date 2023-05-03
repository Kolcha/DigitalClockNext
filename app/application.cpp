#include "application.hpp"

#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

#include "settings/app_config.hpp"
#include "settings/backend_qsettings.hpp"

class ConfigStorageType final : public ConfigStorage<QString, QString, QVariant> {
public:
  using ConfigStorage::ConfigStorage;
};

using namespace Qt::Literals::StringLiterals;

// constructor and destructor are required here to get
// std::unique_ptr to work with forward declarations
Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
{
  setApplicationName(u"Digital Clock Next"_s);
  setApplicationDisplayName(applicationName());
  setApplicationVersion(u"1.0.0"_s);
  setOrganizationName(u"Nick Korotysh"_s);
  setOrganizationDomain(u"kolcha.github.com"_s);
  setDesktopFileName(u"com.github.kolcha.DigitalClockNext"_s);
}

Application::~Application() = default;

void Application::init()
{
  initConfig();
  initTray();
}

// TODO: consider to move to own files
void Application::showSettingsDialog()
{
}

void Application::showAboutDialog()
{
}

void Application::initConfig()
{
  // TODO: use file for now, switch to default later
  auto backend = std::make_unique<BackendQSettings>(u"settings.ini"_s);
  _config_storage = std::make_shared<ConfigStorageType>(std::move(backend));
  _app_config = std::make_unique<AppConfig>(_config_storage);
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
