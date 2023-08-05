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

  QApplication::setApplicationVersion(QString::fromLatin1(APP_VERSION));
  QApplication::setWindowIcon(QIcon::fromTheme("clock"));

  a.init();
  return a.exec();
}
