#pragma once

#include "skin/clock_skin.hpp"

#include <utility>

#include "core/layout_builder.hpp"
#include "core/linear_layout.hpp"
#include "core/renderable_item.hpp"
#include "skin/renderable_factory.hpp"

class ClassicSkin : public ClockSkin {
public:
  explicit ClassicSkin(std::shared_ptr<RenderableFactory> provider)
    : _factory(std::move(provider))
  {}

  std::unique_ptr<Layout> layout(QStringView str) override
  {
    _seps.clear();
    auto builder = LayoutBuilder<LinearLayout>(Qt::Horizontal);
    for (const auto& c : str) {
      auto r = _factory->item(c);
      // TODO: implement isSeparator() instead
      if (c == 'o')
        _seps.push_back(r);
      builder.addItem(std::make_unique<RenderableItem>(r));
    }
    return builder.build();
  }

  std::vector<std::shared_ptr<Renderable>> separators() const override { return _seps; }

private:
  std::shared_ptr<RenderableFactory> _factory;
  std::vector<std::shared_ptr<Renderable>> _seps;
};
