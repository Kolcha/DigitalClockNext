#include "application.hpp"
#include "application_private.hpp"

#include "about_dialog.hpp"
#include "settings_dialog.hpp"

using namespace Qt::Literals::StringLiterals;

// constructor and destructor are required here to get
// std::unique_ptr to work with forward declarations
Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
  , _impl(std::make_unique<ApplicationPrivate>())
{
  setApplicationName(u"DigitalClockNext"_s);
  setApplicationDisplayName(u"Digital Clock Next"_s);
  setOrganizationName(u"Nick Korotysh"_s);
  setOrganizationDomain(u"kolcha.github.com"_s);
  setDesktopFileName(u"com.github.kolcha.DigitalClockNext"_s);
}

Application::~Application() = default;

void Application::init()
{
  initConfig();
  initTray();
  initCore();
  createWindows();
}

void Application::showSettingsDialog()
{
  auto w = qobject_cast<ClockWindow*>(sender());
  // TODO: find window on current screen in case of tray icon clicked
  auto dlg = new SettingsDialog(_impl.get(), _impl->window_index(w), w);
  connect(dlg, &QDialog::finished, dlg, &QObject::deleteLater);
  dlg->show();
}

void Application::showAboutDialog()
{
  auto w = qobject_cast<ClockWindow*>(sender());
  // TODO: find window on current screen in case of tray icon clicked
  auto dlg = new AboutDialog(w);
  connect(dlg, &QDialog::finished, dlg, &QObject::deleteLater);
  dlg->show();
}
