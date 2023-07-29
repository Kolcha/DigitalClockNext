#include "legacy_skin_loader.hpp"

#include <algorithm>

#include <QFileInfo>

#include "render/legacy_image_renderable.hpp"

namespace {

static constexpr const char* required_keys[] = {
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

static constexpr const char* const skin_ini = "skin.ini";

} // namespace

std::shared_ptr<SkinResource> LegacyRenderableFactory::createRenderable(const QString& path)
{
  QFileInfo fi(path);
  QString ext = fi.suffix().toLower();

  if (ext == "svg")
    return std::make_shared<SvgImageRenderable>(path);

  if (ext == "png")
    return std::make_shared<RasterImageRenderable>(path);

  return nullptr;
}


void LegacySkinLoader::init()
{
  QFileInfo finfo(path());
  if (!finfo.isDir())
    return;

  QDir skin_dir(finfo.absoluteFilePath());
  if (!skin_dir.exists(skin_ini))
    return;

  QSettings s(skin_dir.filePath(skin_ini), QSettings::IniFormat);
  auto has_empty_value = [&](const auto& k) { return s.value(k).toString().isEmpty(); };
  if (std::ranges::any_of(required_keys, has_empty_value))
    return;

  auto file_exists = [&](const auto& k) { return skin_dir.exists(s.value(k).toString()); };
  if (!std::all_of(required_keys + 1, required_keys + 12, file_exists))
    return;

  loadMeta(skin_dir, s);
  loadFiles(skin_dir, s);

  setValid(meta().contains("name") && _files.size() >= 11);
}

void LegacySkinLoader::loadMeta(const QDir& skin_dir, QSettings& s)
{
  Q_UNUSED(skin_dir);
  s.beginGroup("info");
  const auto meta_keys = s.childKeys();
  std::ranges::for_each(meta_keys, [&](const auto& k) { updateMeta(k, s.value(k)); });
  s.endGroup();
}

void LegacySkinLoader::loadFiles(const QDir& skin_dir, QSettings& s)
{
  auto add_file = [&](QChar ch, const QString& file) {
    if (!file.isEmpty() && skin_dir.exists(file))
      _files[ch] = skin_dir.filePath(file);
  };

  s.beginGroup("files");
  for (int i = 0; i < 10; ++i) {
    auto key = QString("%1").arg(i);
    add_file(key[0], s.value(key).toString());
  }
  add_file(':', s.value("s1").toString());
  add_file(' ', s.value("s2").toString());
  add_file('a', s.value("am").toString());
  add_file('p', s.value("pm").toString());
  s.endGroup();
}
