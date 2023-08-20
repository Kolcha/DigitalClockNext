#include <QIcon>

#include "app/application.hpp"
#include "version.hpp"

int main(int argc, char *argv[])
{
  Application a(argc, argv);

  using namespace Qt::Literals::StringLiterals;
  // set system icon theme as fallback
  auto system_theme = QIcon::themeName();
  QIcon::setThemeName(u"unicons-line"_s);
  QIcon::setFallbackThemeName(system_theme);

  QApplication::setApplicationName(QString::fromLatin1(APP_PROJECT_NAME));
  QApplication::setApplicationDisplayName(u"Digital Clock Next"_s);
  QApplication::setApplicationVersion(QString::fromLatin1(APP_VERSION));
  QApplication::setOrganizationName(u"Nick Korotysh"_s);
  QApplication::setOrganizationDomain(u"kolcha.github.com"_s);
  QApplication::setDesktopFileName(APP_IDENTIFIER);
  QApplication::setWindowIcon(QIcon::fromTheme("clock"));
#ifdef Q_OS_MACOS
  QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
  QApplication::setQuitOnLastWindowClosed(false);

  a.init();
  return a.exec();
}
