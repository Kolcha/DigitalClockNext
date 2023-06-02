#include "legacy_skin_validator.hpp"

#include <algorithm>

#include <QDir>
#include <QFileInfo>
#include <QSettings>

std::optional<QString> tryLegacySkin(const QString& path)
{
  using namespace Qt::Literals::StringLiterals;

  QFileInfo finfo(path);
  if (!finfo.isDir())
    return std::nullopt;

  QDir dir(finfo.absoluteFilePath());
  if (!dir.exists(u"skin.ini"_s))
    return std::nullopt;

  QSettings cfg(dir.absoluteFilePath(u"skin.ini"_s), QSettings::IniFormat);
  constexpr const char* required_keys[] = {
    "info/name",
    "files/1",
    "files/2",
    "files/3",
    "files/4",
    "files/5",
    "files/6",
    "files/7",
    "files/8",
    "files/9",
    "files/0",
    "files/s1",
  };
  if (!std::ranges::all_of(required_keys, [&](const auto& k) { return cfg.contains(k); }))
    return std::nullopt;

  auto name = cfg.value("info/name").toString();
  if (name.isEmpty())
    return std::nullopt;

  return name;
}
