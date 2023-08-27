#pragma once

#include "skin/classic_skin_loader.hpp"
#include "skin/renderable_factory.hpp"

#include <QDir>
#include <QHash>

class LegacyRenderableFactory final : public RenderableFactory {
public:
  explicit LegacyRenderableFactory(const QHash<QChar, QString>& files);

  std::shared_ptr<SkinResource> item(QChar ch) const override;

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
  {
    init(path);
  }

  std::unique_ptr<ClassicSkin> skin() const override
  {
    if (!valid())
      return nullptr;

    auto factory = std::make_unique<LegacyRenderableFactory>(_files);
    return std::make_unique<ClassicSkin>(std::move(factory));
  }

private:
  void init(const QString& skin_root);

  void loadMeta(const QDir& skin_dir);
  void loadFiles(const QDir& skin_dir);

private:
  QHash<QChar, QString> _files;
};
