#include <QIcon>

#include "app/application.hpp"

int main(int argc, char *argv[])
{
  Application a(argc, argv);

  using namespace Qt::Literals::StringLiterals;
  // set system icon theme as fallback
  auto system_theme = QIcon::themeName();
  QIcon::setThemeName(u"unicons-line"_s);
  QIcon::setFallbackThemeName(system_theme);

  a.init();
  return a.exec();
}
