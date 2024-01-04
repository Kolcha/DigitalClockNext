#include "application.hpp"
#include "application_private.hpp"

#include <QDesktopServices>
#include <QLocale>

#include "about_dialog.hpp"
#include "settings/settings_dialog.hpp"

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
  initUpdater();
  createWindows();
}

void Application::showSettingsDialog()
{
  auto w = qobject_cast<ClockWindow*>(sender());
  auto widx = w? _impl->window_index(w) : 0;
  if (!w) w = _impl->window(widx).get();
  Q_ASSERT(w);
  w->raise();
  w->activateWindow();
  const auto tag = 0x73646c67 + widx;
  auto dlg = _impl->maybeCreateAndShowDialog<SettingsDialog>(tag, _impl.get(), widx);
  auto reconfigure = [=, this]() {
    if (_impl->app_config()->global().getConfigPerWindow())
      _impl->configureWindow(widx);
    else
      std::ranges::for_each(_impl->windows(), [this](auto& w) { _impl->configureWindow(w.get()); });
  };
  connect(dlg, &QDialog::rejected, _impl.get(), reconfigure);
  connect(dlg, &QObject::destroyed, w, &QWidget::raise);
}

void Application::showAboutDialog()
{
  _impl->maybeCreateAndShowDialog<AboutDialog>(0x41697269);
}

void Application::handleNewVersion(QVersionNumber version, QDate date, QUrl link)
{
  // *INDENT-OFF*
  _impl->tray_icon()->showMessage(
        tr("%1 Update").arg(qApp->applicationDisplayName()),
        tr("Update available (%1, %2). Click this message to download.")
            .arg(version.toString(), QLocale::system().toString(date, QLocale::ShortFormat)),
        QSystemTrayIcon::Warning);
  connect(_impl->tray_icon().get(), &QSystemTrayIcon::messageClicked,
          [=] () { QDesktopServices::openUrl(link); });
  // *INDENT-ON*
  auto on_msg_clicked = [this]() { disconnect(_impl->tray_icon().get(), &QSystemTrayIcon::messageClicked, nullptr, nullptr); };
  connect(_impl->tray_icon().get(), &QSystemTrayIcon::messageClicked, this, on_msg_clicked);
}

void Application::handleUpdateError(QString error_msg)
{
  // *INDENT-OFF*
  _impl->tray_icon()->showMessage(
        tr("%1 Update").arg(qApp->applicationDisplayName()),
        tr("Update error. %1").arg(error_msg),
        QSystemTrayIcon::Critical);
  // *INDENT-ON*
}
