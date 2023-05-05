#include "skin_manager.hpp"

#include "clock/legacy_skin_format_extension.hpp"
#include "render/identity_effect.hpp"
#include "render/texturing_effect.hpp"
#include "skin/legacy_skin_loader.hpp"

SkinManagerImpl::SkinManagerImpl(ApplicationPrivate* app, QObject* parent)
  : SkinManager(parent)
  , _app(app)
{}

SkinManager::SkinPtr SkinManagerImpl::loadSkin(const QString& skin_name) const
{
  // only legacy skins for now
  // TODO: support at least text skins.
  //       is it worth? they are too unique...
  //       maybe separate method for them...
  return loadLegacySkin(skin_name);
}

void SkinManagerImpl::configureSkin(const SkinPtr& skin, std::size_t i) const
{
  // only classic skins for now
  if (auto cskin = std::dynamic_pointer_cast<ClassicSkin>(skin)) {
    configureClassicSkin(cskin, i);
    return;
  }
}

SkinManagerImpl::ClassicSkinPtr SkinManagerImpl::loadLegacySkin(const QString& skin_name) const
{
  LegacySkinLoader loader;
  auto skin = loader.load(skin_name);

  auto flashing_dots_ext = std::make_shared<LegacySkinFormatExtension>();
  connect(_app->time_source().get(), &TimeSource::halfSecondUpdate,
          flashing_dots_ext.get(), &LegacySkinFormatExtension::setSeparatorVisible);
  skin->formatter()->addExtension(std::move(flashing_dots_ext));

  return skin;
}

void SkinManagerImpl::configureClassicSkin(const ClassicSkinPtr& skin, std::size_t i) const
{
  QConicalGradient g1(0.5, 0.5, 45.0);
  g1.setStops({
    {0.00, {170,   0,   0}},  // #aa0000
    {0.20, {  0,  85, 255}},  // #0055ff
    {0.45, {  0, 170,   0}},  // #00aa00
    {0.65, {255, 255,   0}},  // #ffff00
    {1.00, {170,   0,   0}},  // #aa0000
  });
  g1.setCoordinateMode(QGradient::ObjectMode);
  auto effect1 = std::make_shared<TexturingEffect>();
  effect1->setBrush(g1);

  QLinearGradient g2(0., 0., 0., 1.);
  g2.setColorAt(0.0, Qt::transparent);
  g2.setColorAt(0.9, Qt::transparent);
  g2.setColorAt(1.0, Qt::magenta);
  g2.setCoordinateMode(QGradient::ObjectMode);
  auto effect2 = std::make_shared<TexturingEffect>();
  effect2->setBrush(g2);

  skin->addItemEffect(effect1);
  skin->addLayoutEffect(std::make_unique<IdentityEffect>());
  skin->addLayoutEffect(effect2);

  skin->formatter()->setFormat("hh:mm:ss a");
}