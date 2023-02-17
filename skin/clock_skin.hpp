#pragma once

#include <memory>
#include <ranges>
#include <vector>

#include <QStringView>

#include "core/renderable_item.hpp"
#include "render/layout_renderable.hpp"


class ClockRenderable : public LayoutRenderable {
  using RendererPtr = std::shared_ptr<LayoutRenderer>;
  using LayoutPtr = std::unique_ptr<Layout>;
  using Separators = std::vector<std::shared_ptr<Renderable>>;

public:
  ClockRenderable(RendererPtr renderer, LayoutPtr layout, Separators separators)
    : LayoutRenderable(std::move(renderer), std::move(layout))
    , _separators(std::move(separators))
  {
  }

  void setSeparatorsVisible(bool visible)
  {
    std::ranges::for_each(_separators, [=](const auto& s) { s->setVisible(visible); });
  }

  bool areSeparatorsVisible() const
  {
    return std::ranges::all_of(_separators, [](const auto& s) { return s->isVisible(); });
  }

protected:
  void setLayout(LayoutPtr layout, Separators separators)
  {
    LayoutRenderable::setLayout(std::move(layout));
    _separators = std::move(separators);
  }

  const auto& separators() const noexcept { return _separators; }

private:
  Separators _separators;
};


class ClockSkin {
public:
  virtual ~ClockSkin() = default;

  // TODO: replace with date/time type, string for now
  virtual std::unique_ptr<RenderableItem> process(QStringView dt) = 0;
};
