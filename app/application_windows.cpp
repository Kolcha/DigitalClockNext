#include "application.hpp"
#include "application_private.hpp"

#include "app/clock_window.hpp"
#include "clock/legacy_skin_format_extension.hpp"
#include "render/identity_effect.hpp"
#include "render/texturing_effect.hpp"
#include "skin/legacy_skin_loader.hpp"

void ApplicationPrivate::initWindows()
{
  // TODO: create multiple windows, for now only one is supported
  // TODO: apply time zone, for now local time zone is hardcoded
  // TODO: implement skin manager which will be responsible for skin loading
  //       and what about skin configuration? high-level interfaces don't know about possible options...
  //       only this skin manager knows what was actually created and return that object
  //       consider "skin type" enum, and and apply configuration (also change UI) depending on it
  LegacySkinLoader loader;
  auto skin = loader.load("floral_digits");

  auto flashing_dots_ext = std::make_shared<LegacySkinFormatExtension>();
  connect(_time_src.get(), &TimeSource::halfSecondUpdate,
          flashing_dots_ext.get(), &LegacySkinFormatExtension::setSeparatorVisible);
  skin->formatter()->addExtension(std::move(flashing_dots_ext));

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

  auto wnd = std::make_unique<ClockWindow>(std::move(skin), _time_src->now().toLocalTime());
  connect(_time_src.get(), &TimeSource::timeChanged, wnd.get(), &ClockWindow::setDateTime);
  connect(_time_src.get(), &TimeSource::halfSecondUpdate, wnd.get(), &ClockWindow::setSeparatorVisible);
  wnd->show();
  _windows.emplace_back(std::move(wnd));
}

void Application::createWindows()
{
  _impl->initWindows();
}
