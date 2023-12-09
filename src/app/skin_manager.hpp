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

#include "application_private.hpp"

#include "classic_skin.hpp"

// skin manager is "integral part" of application
// it does "all magic" related to skin configuration
// so it needs an access to internal app structures
class SkinManagerImpl final : public SkinManager
{
  Q_OBJECT

public:
  explicit SkinManagerImpl(ApplicationPrivate* app, QObject* parent = nullptr);

  SkinPtr loadSkin(const QFont& font) const override;
  SkinPtr loadSkin(const QString& skin_name) const override;
  SkinPtr loadSkin(std::size_t i) const override;
  void configureSkin(const SkinPtr& skin, std::size_t i) const override;
  QStringList availableSkins() const override;

public slots:
  void findSkins() override;

private:
  using ClassicSkinPtr = std::shared_ptr<ClassicSkin>;
  ClassicSkinPtr loadLegacySkin(const QString& skin_path) const;

  void configureClassicSkin(const ClassicSkinPtr& skin, std::size_t i) const;

private:
  ApplicationPrivate* _app;

  enum class SkinType {
    Legacy,
  };

  struct LoaderInfo {
    SkinType type;
    QString path;
  };

  QHash<QString, LoaderInfo> _skins;
};
