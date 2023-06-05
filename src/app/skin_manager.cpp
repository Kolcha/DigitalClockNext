#include "skin_manager.hpp"

#include <optional>

#include <QDir>

#include "clock/legacy_skin_extension.hpp"
#include "render/background_effect.hpp"
#include "render/identity_effect.hpp"
#include "render/texturing_effect.hpp"
#include "skin/char_renderable_factory.hpp"
#include "skin/legacy_skin_loader.hpp"

namespace {

std::optional<QString> tryLegacySkin(const QString& path)
{
  LegacySkinLoader loader(path);
  if (loader.valid())
    return loader.meta()["name"].toString();
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
  auto provider = std::make_shared<QCharRenderableFactory>(font);
//  provider->setSeparators("o");
  auto skin = std::make_shared<ClassicSkin>(std::move(provider));
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
  return nullptr;
}

SkinManager::SkinPtr SkinManagerImpl::loadSkin(std::size_t i) const
{
  const auto& cfg = _app->app_config()->window(i);
  auto skin = cfg.appearance().getUseFontInsteadOfSkin()
              ? loadSkin(cfg.state().getTextSkinFont())
              : loadSkin(cfg.state().getLastUsedSkin());
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
  // TODO: platform/distribution-specific search paths
  const auto search_paths = {
    u"."_s,   // only current path for now
  };

  constexpr std::pair<SkinType, std::optional<QString>(*)(const QString&)> validators[] = {
    {SkinType::Legacy, &tryLegacySkin},
  };

  for (const auto& path : search_paths) {
    QDir dir(path);
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

  auto flashing_dots_ext = std::make_shared<LegacySkinExtension>();
  connect(_app->time_source().get(), &TimeSource::halfSecondUpdate,
          flashing_dots_ext.get(), &LegacySkinExtension::setSeparatorVisible);
  skin->formatter()->addExtension(std::move(flashing_dots_ext));

  return skin;
}

void SkinManagerImpl::configureClassicSkin(const ClassicSkinPtr& skin, std::size_t i) const
{
  using namespace Qt::Literals::StringLiterals;

  const auto& cfg = _app->app_config()->window(i);

  skin->formatter()->setExtensionEnabled(u"legacy_skin"_s, cfg.appearance().getFlashingSeparator());

  skin->clearItemEffects();
  skin->clearLayoutEffects();

  if (auto brush = cfg.classicSkin().getBackground(); brush.style() != Qt::NoBrush) {
    auto effect = std::make_unique<BackgroundEffect>(std::move(brush));
    if (cfg.classicSkin().getBackgroundPerElement()) {
      skin->addItemEffect(std::move(effect));
      skin->addItemEffect(std::make_unique<IdentityEffect>());
    } else {
      skin->addLayoutEffect(std::move(effect));
      skin->addLayoutEffect(std::make_unique<IdentityEffect>());
    }
  }

  if (auto brush = cfg.classicSkin().getTexture(); brush.style() != Qt::NoBrush) {
    auto effect = std::make_unique<TexturingEffect>(std::move(brush));
    if (cfg.classicSkin().getTexturePerElement()) {
      skin->addItemEffect(std::move(effect));
    } else {
      skin->addLayoutEffect(std::move(effect));
    }
  }

  skin->formatter()->setFormat(cfg.classicSkin().getTimeFormat());
  skin->setOrientation(cfg.classicSkin().getOrientation());
  skin->setSpacing(cfg.classicSkin().getSpacing());
}
