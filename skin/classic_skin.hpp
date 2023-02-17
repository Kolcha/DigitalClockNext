#pragma once

#include "skin/clock_skin.hpp"

#include <memory>

#include "core/layout_builder.hpp"
#include "core/linear_layout.hpp"
#include "core/renderable_item.hpp"
#include "skin/renderable_factory.hpp"
#include "render/composite_effect.hpp"
#include "render/layout_renderer.hpp"


// this is used (and returned) by ClassicClockSkin
// TODO: consider make it "implementation detail"
// TODO: add rendering customization functions
class ClassicSkinRenderable : public ClockRenderable {
public:
  using ClockRenderable::ClockRenderable;

  void setLayout(auto layout, auto separators)
  {
    ClockRenderable::setLayout(std::move(layout), std::move(separators));
  }
};


// TODO: consider to add setter for factory object -
// this will allow to change classic skins only with changing factory
// TODO: add rendering customization functions
class ClassicSkin : public ClockSkin {
public:
  explicit ClassicSkin(std::shared_ptr<RenderableFactory> provider)
    : _factory(std::move(provider))
    , _renderer(std::make_shared<LayoutRenderer>())
    , _item_effects(std::make_shared<CompositeEffect>())
    , _layout_effects(std::make_shared<CompositeEffect>())
  {}

  std::unique_ptr<RenderableItem> process(QStringView str) override
  {
    std::vector<std::shared_ptr<Renderable>> seps;
    auto builder = LayoutBuilder<LinearLayout>(Qt::Horizontal);
    for (const auto& c : str) {
      auto r = _factory->item(c);
      if (!r) {
        qDebug() << "no renderable: ch =" << c;
        continue;
      }
      if (isSeparator(c))
        seps.push_back(r);
      auto item = std::make_unique<RenderableItem>(r);
      item->addEffect(_item_effects);
      builder.addItem(std::move(item));
    }

    auto layout = builder.build();
    auto r = createRenderable(std::move(layout), std::move(seps));
    auto item = std::make_unique<RenderableItem>(r);
    item->addEffect(_layout_effects);
    return item;
  }

  void addItemEffect(std::shared_ptr<Effect> effect)
  {
    _item_effects->addEffect(std::move(effect));
  }

  void addLayoutEffect(std::shared_ptr<Effect> effect)
  {
    _layout_effects->addEffect(std::move(effect));
  }

private:
  bool isSeparator(QChar ch) const
  {
    // TODO: time_format.isSeparator(ch) && factory.isSeparator(ch)
    return _factory->isSeparator(ch);
  }

  std::shared_ptr<ClockRenderable> createRenderable(auto... args)
  {
    if (_widget)
      _widget->setLayout(std::move(args)...);
    else
      _widget = std::make_shared<ClassicSkinRenderable>(_renderer, std::move(args)...);
    return _widget;
  }

private:
  std::shared_ptr<RenderableFactory> _factory;
  std::shared_ptr<LayoutRenderer> _renderer;
  std::shared_ptr<ClassicSkinRenderable> _widget;
  std::shared_ptr<CompositeEffect> _item_effects;
  std::shared_ptr<CompositeEffect> _layout_effects;
};
