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
  const auto widx = _impl->window_index(w);
  // TODO: find window on current screen in case of tray icon clicked
  if (!w) w = _impl->window(_impl->window_index(w)).get();
  w->raise();
  w->activateWindow();
  const auto tag = 0x73646c67 + widx;
  auto dlg = _impl->maybeCreateAndShowDialog<SettingsDialog>(tag, _impl.get(), widx);
  connect(dlg, &QObject::destroyed, w, &QWidget::raise);
}

void Application::showAboutDialog()
{
  _impl->maybeCreateAndShowDialog<AboutDialog>(0x41697269);
}
