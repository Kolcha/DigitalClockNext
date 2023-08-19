#include "skin_manager.hpp"

#include <array>
#include <optional>

#include <QApplication>
#include <QDir>

#include "clock/legacy_skin_extension.hpp"
#include "render/effects/background.hpp"
#include "render/effects/identity.hpp"
#include "render/effects/texturing.hpp"
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

// item bg, texture + layout bg, texture
std::array<QBrush, 4> classicSkinBrushes(const WindowConfig& cfg)
{
  std::array<QBrush, 4> brushes;

  if (auto brush = cfg.classicSkin().getBackground(); brush.style() != Qt::NoBrush) {
    if (cfg.classicSkin().getBackgroundPerElement())
      brushes[0] = brush; // item bg
    else
      brushes[2] = brush; // layout bg
  }

  if (auto brush = cfg.classicSkin().getTexture(); brush.style() != Qt::NoBrush) {
    if (cfg.classicSkin().getTexturePerElement())
      brushes[1] = brush; // item texture
    else
      brushes[3] = brush; // layout texture
  }

  return brushes;
}

struct EffectSurface {
  void(ClassicSkin::*addEffect)(std::shared_ptr<Effect>);
};

constexpr EffectSurface kSurfaceItem {
  &ClassicSkin::addItemEffect,
};

constexpr EffectSurface kSurfaceLayout {
  &ClassicSkin::addLayoutEffect,
};

void configureEffects(QBrush bg, QBrush tx, ClassicSkin& skin, EffectSurface surface)
{
  if (bg.style() == Qt::NoBrush && tx.style() == Qt::NoBrush) {
    (skin.*surface.addEffect)(std::make_unique<IdentityEffect>());
  }
  if (bg.style() != Qt::NoBrush && tx.style() == Qt::NoBrush) {
    (skin.*surface.addEffect)(std::make_unique<BackgroundEffect>(std::move(bg)));
    (skin.*surface.addEffect)(std::make_unique<IdentityEffect>());
  }
  if (bg.style() == Qt::NoBrush && tx.style() != Qt::NoBrush) {
    (skin.*surface.addEffect)(std::make_unique<IdentityEffect>());
    (skin.*surface.addEffect)(std::make_unique<TexturingEffect>(std::move(tx)));
  }
  if (bg.style() != Qt::NoBrush && tx.style() != Qt::NoBrush) {
    (skin.*surface.addEffect)(std::make_unique<BackgroundEffect>(std::move(bg)));
    auto composite_effect = std::make_unique<CompositeEffect>();
    composite_effect->addEffect(std::make_unique<IdentityEffect>());
    composite_effect->addEffect(std::make_unique<TexturingEffect>(std::move(tx)));
    (skin.*surface.addEffect)(std::move(composite_effect));
  }
}

void loadClassicSkinEffects(ClassicSkin& skin, const WindowConfig& cfg)
{
  auto [item_bg, item_tx, layout_bg, layout_tx] = classicSkinBrushes(cfg);
  configureEffects(std::move(item_bg), std::move(item_tx), skin, kSurfaceItem);
  configureEffects(std::move(layout_bg), std::move(layout_tx), skin, kSurfaceLayout);
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
  const auto search_paths = {
    u":/skins"_s,
#ifdef Q_OS_MACOS
    qApp->applicationDirPath() + u"/../Resources/skins"_s,
#else
    qApp->applicationDirPath() + u"/skins"_s,
#endif
#ifdef Q_OS_LINUX
    u"/usr/share/digitalclock4/skins"_s,
    u"/usr/share/DigitalClockNext/skins"_s,
    u"/usr/local/share/digitalclock4/skins"_s,
    u"/usr/local/share/DigitalClockNext/skins"_s,
    QDir::home().absoluteFilePath(u".local/share/digitalclock4/skins"_s),
    QDir::home().absoluteFilePath(u".local/share/DigitalClockNext/skins"_s),
#endif
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

  loadClassicSkinEffects(*skin, cfg);

  skin->formatter()->setFormat(cfg.classicSkin().getTimeFormat());
  skin->setOrientation(cfg.classicSkin().getOrientation());
  skin->setSpacing(cfg.classicSkin().getSpacing());
}
