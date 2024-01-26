/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "classic_skin_loader.hpp"
#include "resource_factory.hpp"

#include <optional>

#include <QDir>
#include <QHash>

struct GlyphGeometryRaw {
  std::optional<qreal> x;
  std::optional<qreal> y;
  std::optional<qreal> w;
  std::optional<qreal> h;
  std::optional<qreal> ax;
  std::optional<qreal> ay;
};

using SkinFileInfo = QPair<QString, GlyphGeometryRaw>;
using SkinFilesMap = QHash<QChar, SkinFileInfo>;

class ImageResourceFactory final : public ResourceFactory {
public:
  explicit ImageResourceFactory(const SkinFilesMap& files);

  bool supportsSeparatorAnimation() const noexcept { return _has_2_seps; }

  qreal ascent() const noexcept override { return -_min_y; }
  qreal descent() const noexcept override { return _max_y; }

protected:
  std::shared_ptr<Resource> create(char32_t ch) const override;

private:
  QHash<QChar, std::shared_ptr<Resource>> _resources;
  bool _has_2_seps = false;
  qreal _min_y = 0;
  qreal _max_y = 0;
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

    auto factory = std::make_unique<ImageResourceFactory>(_files);
    bool supports_separator_animation = factory->supportsSeparatorAnimation();
    auto skin = std::make_unique<ClassicSkin>(std::move(factory));
    skin->setSupportsGlyphBaseHeight(true);
    skin->setSupportsSeparatorAnimation(supports_separator_animation);
    return skin;
  }

private:
  void init(const QString& skin_root);

  void loadMeta(const QDir& skin_dir);
  void loadFiles(const QDir& skin_dir);

private:
  SkinFilesMap _files;
};
