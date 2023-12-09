/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "classic_skin_loader.hpp"
#include "renderable_factory.hpp"

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

class LegacyRenderableFactory final : public RenderableFactory {
public:
  explicit LegacyRenderableFactory(const SkinFilesMap& files);

  std::shared_ptr<SkinResource> item(QChar ch) const override;

  bool supportsSeparatorAnimation() const noexcept { return _has_2_seps; }

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
    bool supports_separator_animation = factory->supportsSeparatorAnimation();
    auto skin = std::make_unique<ClassicSkin>(std::move(factory));
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
