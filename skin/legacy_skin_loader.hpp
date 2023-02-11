#pragma once

#include "skin/classic_skin_loader.hpp"
#include "skin/renderable_factory.hpp"

#include <QFileInfo>
#include <QDir>
#include <QHash>
#include <QSettings>

#include "skin/legacy_image_renderable.hpp"

class LegacyRenderableFactory final : public RenderableFactory {
public:
  explicit LegacyRenderableFactory(const QHash<QChar, QString>& files)
  {
    for (auto iter = files.begin(); iter != files.end(); ++iter)
      _resources[iter.key()] = createRenderable(iter.value());
    _has_2_seps = _resources.contains(':') && _resources.contains(' ');
  }

  std::shared_ptr<Renderable> item(QChar ch) const override
  {
    if (RenderableFactory::isSeparator(ch) && ch != ' ')
      ch = ':';
    auto iter = _resources.find(ch);
    return iter != _resources.end() ? iter.value() : nullptr;
  }

  bool isSeparator(QChar ch) const override
  {
    return !_has_2_seps && RenderableFactory::isSeparator(ch);
  }

private:
  static std::shared_ptr<Renderable> createRenderable(const QString& path)
  {
    QFileInfo fi(path);
    QString ext = fi.suffix().toLower();

    if (ext == "svg")
      return std::make_shared<SvgImageRenderable>(path);

    if (ext == "png")
      return std::make_shared<RasterImageRenderable>(path);

    return nullptr;
  }

private:
  QHash<QChar, std::shared_ptr<Renderable>> _resources;
  bool _has_2_seps = false;
};


class LegacySkinLoader final : public ClassicSkinLoader {
public:
  std::unique_ptr<ClassicSkin> load(const QString& path) const override
  {
    QDir skin_dir(path);

    if (!skin_dir.exists(skin_ini))
      return nullptr;

    auto files = getSkinFiles(skin_dir);
    auto factory = std::make_unique<LegacyRenderableFactory>(files);
    return std::make_unique<ClassicSkin>(std::move(factory));
  }

private:
  static QHash<QChar, QString> getSkinFiles(const QDir& skin_dir)
  {
    QHash<QChar, QString> files;

    auto add_file = [&](QChar ch, const QString& file) {
      if (!file.isEmpty() && skin_dir.exists(file))
        files[ch] = skin_dir.filePath(file);
    };

    QSettings s(skin_dir.filePath(skin_ini), QSettings::IniFormat);
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

    return files;
  }

  static constexpr const char* const skin_ini = "skin.ini";
};
