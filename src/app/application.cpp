#include "application.hpp"
#include "application_private.hpp"

#include "about_dialog.hpp"
#include "settings_dialog.hpp"

// constructor and destructor are required here to get
// std::unique_ptr to work with forward declarations
Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
  , _impl(std::make_unique<ApplicationPrivate>())
{
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
  if (!w) w = _impl->window(_impl->window_index(w)).get();
  w->raise();
  w->activateWindow();
  connect(dlg, &QObject::destroyed, w, &QWidget::raise);
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
  dlg->raise();
}
