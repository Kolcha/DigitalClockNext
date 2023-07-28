#pragma once

#include "skin/clock_skin.hpp"

#include <memory>

#include "clock/datetime_formatter.hpp"
#include "core/linear_layout.hpp"
#include "core/rendering.hpp"
#include "skin/renderable_factory.hpp"
#include "render/composite_effect.hpp"


// this is used (and returned) by ClassicClockSkin
// TODO: consider make it "implementation detail"
class ClassicSkinRenderable : public ClockRenderable {
  friend class ClassicSkin;
public:
  using ClockRenderable::ClockRenderable;
};


// TODO: consider to add setter for factory object -
// this will allow to change classic skins only with changing factory
class ClassicSkin : public ClockSkin {
public:
  explicit ClassicSkin(std::shared_ptr<RenderableFactory> provider)
    : _factory(std::move(provider))
    , _layout_alg(std::make_shared<LinearLayout>())
    , _item_effects(std::make_shared<CompositeEffect>())
    , _layout_effects(std::make_shared<CompositeEffect>())
    , _settings(std::make_unique<ClassicSkinSettings>())
    , _formatter(std::make_unique<DateTimeFormatter>("hh:mm a"))
  {}

  std::shared_ptr<ClockRenderable> process(const QDateTime& dt) override
  {
    auto layout = std::make_shared<ClassicSkinRenderable>();
    std::vector<std::shared_ptr<SkinElement>> seps;
    // TODO: is it really required to set settigns here every time ?
    _layout_alg->setOrientation(_settings->orientation);
    _layout_alg->setSpacing(_settings->spacing);
    layout->setAlgorithm(_layout_alg);

    const auto str = _formatter->process(dt);
    for (const auto& c : str) {
      auto r = _factory->item(c);
      if (!r) {
        continue;
      }

      auto item = std::make_shared<SimpleSkinElement>(std::move(r));
      if (isSeparator(c))
        seps.push_back(item);
      item->addEffect(_item_effects);
      layout->addElement(std::move(item));
    }

    layout->setSeparators(std::move(seps));
    layout->addEffect(_layout_effects);
    return layout;
  }

  void setOrientation(Qt::Orientation orientation)
  {
    _settings->orientation = orientation;
  }

  void setSpacing(qreal spacing)
  {
    _settings->spacing = spacing;
  }

  void addItemEffect(std::shared_ptr<Effect> effect)
  {
    _item_effects->addEffect(std::move(effect));
  }

  void addLayoutEffect(std::shared_ptr<Effect> effect)
  {
    _layout_effects->addEffect(std::move(effect));
  }

  void clearItemEffects()
  {
    _item_effects->clearEffects();
  }

  void clearLayoutEffects()
  {
    _layout_effects->clearEffects();
  }

  const auto& formatter() const noexcept { return _formatter; }

private:
  bool isSeparator(QChar ch) const
  {
    return _formatter->isSeparator(ch) && _factory->isSeparator(ch);
  }

private:
  struct ClassicSkinSettings {
    Qt::Orientation orientation = Qt::Horizontal;
    qreal spacing = 0;
    // TODO: move effects here
  };

  std::shared_ptr<RenderableFactory> _factory;
  std::shared_ptr<LinearLayout> _layout_alg;
  std::shared_ptr<CompositeEffect> _item_effects;
  std::shared_ptr<CompositeEffect> _layout_effects;
  std::unique_ptr<ClassicSkinSettings> _settings;
  std::unique_ptr<DateTimeFormatter> _formatter;
};
