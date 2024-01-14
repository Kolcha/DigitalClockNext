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

#include "skin_manager.hpp"

#include <algorithm>
#include <iterator>
#include <optional>

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include "font_resource.hpp"
#include "error_skin.hpp"
#include "legacy_skin_loader.hpp"

namespace {

std::optional<QString> tryLegacySkin(const QString& path)
{
  LegacySkinLoader loader(path);
  if (loader.valid())
    return loader.title();
  return std::nullopt;
}

} // namespace

SkinManagerImpl::SkinManagerImpl(ApplicationPrivate* app, QObject* parent)
  : SkinManager(parent)
  , _app(app)
{
  findSkins();
}

SkinManager::SkinPtr SkinManagerImpl::loadSkin(const QFont& font) const
{
  auto provider = std::make_shared<FontResourceFactory>(font);
  auto skin = std::make_shared<ClassicSkin>(std::move(provider));
  skin->setSupportsCustomSeparator(true);
  return skin;
}

SkinManager::SkinPtr SkinManagerImpl::loadSkin(const QString& skin_name) const
{
  auto iter = _skins.find(skin_name);
  if (iter != _skins.end()) {
    switch (iter.value().type) {
      case SkinType::Legacy:
        return loadLegacySkin(iter.value().path);
    }
  }
  return std::make_unique<ErrorSkin>();
}

SkinManager::SkinPtr SkinManagerImpl::loadSkin(std::size_t i) const
{
  const auto& cfg = _app->app_config()->window(i);
  auto skin = cfg.appearance().getUseFontInsteadOfSkin()
              ? loadSkin(cfg.state().getTextSkinFont())
              : loadSkin(cfg.state().getLastUsedSkin());
  if (!skin && !cfg.appearance().getUseFontInsteadOfSkin()) {
    cfg.appearance().setUseFontInsteadOfSkin(true);
    return loadSkin(i);
  }
  configureSkin(skin, i);
  return skin;
}

void SkinManagerImpl::configureSkin(const SkinPtr& skin, std::size_t i) const
{
  // only classic skins for now
  if (auto cskin = std::dynamic_pointer_cast<ClassicSkin>(skin)) {
    configureClassicSkin(cskin, i);
    return;
  }
}

QStringList SkinManagerImpl::availableSkins() const
{
  QStringList skins = _skins.keys();
  skins.sort();
  return skins;
}

void SkinManagerImpl::findSkins()
{
  _skins.clear();

  using namespace Qt::Literals::StringLiterals;
  QStringList search_paths = {
    u":/skins"_s,
#ifdef Q_OS_LINUX
    qApp->applicationDirPath() + u"/skins"_s,
    u"/usr/share/digitalclock4/skins"_s,
    u"/usr/share/DigitalClockNext/skins"_s,
    u"/usr/local/share/digitalclock4/skins"_s,
    u"/usr/local/share/DigitalClockNext/skins"_s,
    QDir::home().absoluteFilePath(u".local/share/digitalclock4/skins"_s),
    QDir::home().absoluteFilePath(u".local/share/DigitalClockNext/skins"_s),
#endif
  };

  const auto standard_paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
  std::transform(standard_paths.rbegin(), standard_paths.rend(),
                 std::back_inserter(search_paths),
                 [](const QString& path) { return QDir(path).absoluteFilePath(u"skins"_s); });

  constexpr std::pair<SkinType, std::optional<QString>(*)(const QString&)> validators[] = {
    {SkinType::Legacy, &tryLegacySkin},
  };

  for (const auto& path : std::as_const(search_paths)) {
    QDir dir(path);
    if (!dir.exists())
      continue;
    const auto items = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const auto& item : items) {
      auto skin_path = dir.absoluteFilePath(item);
      for (const auto& [type, validator] : validators) {
        if (auto name = (*validator)(skin_path)) {
          _skins[*name] = {type, skin_path};
          break;
        }
      }
    }
  }
}

SkinManagerImpl::ClassicSkinPtr SkinManagerImpl::loadLegacySkin(const QString& skin_path) const
{
  LegacySkinLoader loader(skin_path);
  auto skin = loader.skin();
  return skin;
}

void SkinManagerImpl::configureClassicSkin(const ClassicSkinPtr& skin, std::size_t i) const
{
  using namespace Qt::Literals::StringLiterals;

  const auto& cfg = _app->app_config()->window(i);

  skin->setTexturePerElement(cfg.classicSkin().getTexturePerElement());
  skin->setTextureStretch(cfg.classicSkin().getTextureStretch());
  skin->setTexture(cfg.classicSkin().getTexture());
  skin->setBackgroundPerElement(cfg.classicSkin().getBackgroundPerElement());
  skin->setBackgroundStretch(cfg.classicSkin().getBackgroundStretch());
  skin->setBackground(cfg.classicSkin().getBackground());

  skin->setFormat(cfg.classicSkin().getTimeFormat());
  skin->setOrientation(cfg.classicSkin().getOrientation());
  skin->setSpacing(cfg.classicSkin().getSpacing());
  skin->setCustomSeparators(cfg.classicSkin().getCustomSeparators());

  skin->setIgnoreAdvanceX(cfg.classicSkin().getIgnoreAdvanceX());
  skin->setIgnoreAdvanceY(cfg.classicSkin().getIgnoreAdvanceY());

  if (_app->app_config()->global().getEnableDebugOptions()) {
    skin->setItemDebugFlags(_app->app_config()->debug().getItemDebugFlags());
    skin->setLayoutDebugFlags(_app->app_config()->debug().getLayoutDebugFlags());
    skin->setDebugTopLevelLayout(_app->app_config()->debug().getEnableTopLevelDebug());
    skin->setCachingEnabled(!_app->app_config()->debug().getDisableCaching());
  }
}
