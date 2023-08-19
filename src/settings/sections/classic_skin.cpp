#include "classic_skin.hpp"

#include <QLocale>

QString ClassicSkinConfig::default_time_format()
{
  return QLocale::system().timeFormat(QLocale::ShortFormat);
}
