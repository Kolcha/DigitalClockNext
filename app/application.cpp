#include "application.hpp"

#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

#include "app/clock_window.hpp"
#include "clock/time_source.hpp"
#include "render/identity_effect.hpp"
#include "render/texturing_effect.hpp"
#include "settings/app_config.hpp"
#include "settings/backend_qsettings.hpp"
#include "skin/legacy_skin_loader.hpp"

class ConfigStorageType final : public ConfigStorage<QString, QString, QVariant>
{
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
  _time_src = std::make_unique<TimeSource>();
  createWindows();
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
  // TODO: remove using namespace from config headers
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

void Application::createWindows()
{
  // TODO: create multiple windows, for now only one is supported
  // TODO: apply time zone, for now local time zone is hardcoded
  // TODO: implement skin manager which will be responsible for skin loading
  //       and what about skin configuration? high-level interfaces don't know about possible options...
  //       only this skin manager knows what was actually created and return that object
  //       consider "skin type" enum, and and apply configuration (also change UI) depending on it
  LegacySkinLoader loader;
  auto skin = loader.load("floral_digits");

  QConicalGradient g1(0.5, 0.5, 45.0);
  g1.setStops({
    {0.00, {170,   0,   0}},  // #aa0000
    {0.20, {  0,  85, 255}},  // #0055ff
    {0.45, {  0, 170,   0}},  // #00aa00
    {0.65, {255, 255,   0}},  // #ffff00
    {1.00, {170,   0,   0}},  // #aa0000
  });
  g1.setCoordinateMode(QGradient::ObjectMode);
  auto effect1 = std::make_shared<TexturingEffect>();
  effect1->setBrush(g1);

  QLinearGradient g2(0., 0., 0., 1.);
  g2.setColorAt(0.0, Qt::transparent);
  g2.setColorAt(0.9, Qt::transparent);
  g2.setColorAt(1.0, Qt::magenta);
  g2.setCoordinateMode(QGradient::ObjectMode);
  auto effect2 = std::make_shared<TexturingEffect>();
  effect2->setBrush(g2);

  skin->addItemEffect(effect1);
  skin->addLayoutEffect(std::make_unique<IdentityEffect>());
  skin->addLayoutEffect(effect2);

  skin->formatter()->setFormat("hh:mm:ss a");

  auto wnd = std::make_unique<ClockWindow>(std::move(skin), _time_src->now().toLocalTime());
  connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::setDateTime);
  connect(_time_src.get(), &TimeSource::halfSecondUpdate, wnd.get(), &ClockWindow::setSeparatorVisible);
  wnd->show();
  _windows.emplace_back(std::move(wnd));
}
