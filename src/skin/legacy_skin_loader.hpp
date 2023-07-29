#pragma once

#include "skin/classic_skin_loader.hpp"
#include "skin/renderable_factory.hpp"

#include <QDir>
#include <QHash>
#include <QSettings>

class LegacyRenderableFactory final : public RenderableFactory {
public:
  explicit LegacyRenderableFactory(const QHash<QChar, QString>& files)
  {
    for (auto iter = files.begin(); iter != files.end(); ++iter)
      _resources[iter.key()] = createRenderable(iter.value());
    _has_2_seps = _resources.contains(':') && _resources.contains(' ');
  }

  std::shared_ptr<SkinResource> item(QChar ch) const override
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
  static std::shared_ptr<SkinResource> createRenderable(const QString& path);

private:
  QHash<QChar, std::shared_ptr<SkinResource>> _resources;
  bool _has_2_seps = false;
};


class LegacySkinLoader final : public ClassicSkinLoader {
public:
  explicit LegacySkinLoader(const QString& path)
    : ClassicSkinLoader(path)
  {
    init();
  }

  std::unique_ptr<ClassicSkin> skin() const override
  {
    if (!valid())
      return nullptr;

    auto factory = std::make_unique<LegacyRenderableFactory>(_files);
    return std::make_unique<ClassicSkin>(std::move(factory));
  }

private:
  void init();

  void loadMeta(const QDir& skin_dir, QSettings& s);
  void loadFiles(const QDir& skin_dir, QSettings& s);

private:
  QHash<QChar, QString> _files;
};
