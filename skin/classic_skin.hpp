#pragma once

#include "skin/clock_skin.hpp"

#include <memory>

#include "core/layout_builder.hpp"
#include "core/linear_layout.hpp"
#include "core/renderable_item.hpp"
#include "skin/renderable_factory.hpp"
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

protected:
  void renderImpl(QPainter* p) const override
  {
    _renderer.render(layout().get(), p);
  }

private:
  LayoutRenderer _renderer;
};


// TODO: consider to add setter for factory object -
// this will allow to change classic skins only with changing factory
// TODO: add rendering customization functions
class ClassicSkin : public ClockSkin {
public:
  explicit ClassicSkin(std::shared_ptr<RenderableFactory> provider)
    : _factory(std::move(provider))
  {}

  std::shared_ptr<ClockRenderable> process(QStringView str) override
  {
    std::vector<std::shared_ptr<Renderable>> seps;
    auto builder = LayoutBuilder<LinearLayout>(Qt::Horizontal);
    for (const auto& c : str) {
      auto r = _factory->item(c);
      // TODO: implement isSeparator() instead
      if (c == 'o')
        seps.push_back(r);
      builder.addItem(std::make_unique<RenderableItem>(r));
    }

    auto layout = builder.build();

    if (_widget) {
      _widget->setLayout(std::move(layout), std::move(seps));
    } else {
      _widget = std::make_shared<ClassicSkinRenderable>(std::move(layout), std::move(seps));
    }

    return _widget;
  }

private:
  std::shared_ptr<RenderableFactory> _factory;
  std::shared_ptr<ClassicSkinRenderable> _widget;
};
