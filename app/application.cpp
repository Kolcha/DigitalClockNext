#include "application.hpp"
#include "application_private.hpp"

using namespace Qt::Literals::StringLiterals;

// constructor and destructor are required here to get
// std::unique_ptr to work with forward declarations
Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
  , _impl(std::make_unique<ApplicationPrivate>())
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
  initCore();
  createWindows();
}

// TODO: consider to move to own files
void Application::showSettingsDialog()
{
}

void Application::showAboutDialog()
{
}
