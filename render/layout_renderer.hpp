#pragma once

#include <functional>
#include <memory>
#include <ranges>
#include <vector>

#include <QPainter>

#include "core/layout.hpp"
#include "core/renderable_item.hpp"
#include "render/state_guard.hpp"

using RenderItemFn = std::function<void(const LayoutItem*, QPainter*)>;

class Effect {
public:
  virtual ~Effect() = default;

  virtual void apply(const LayoutItem* item, QPainter* p, RenderItemFn r) const = 0;
};


class LayoutRenderer {
  using Effects = std::vector<std::shared_ptr<Effect>>;

public:
  void render(const LayoutItem* item, QPainter* p) const
  {
    // calls recursive function
    render_item(item, p);
  }

  void addItemEffect(std::shared_ptr<Effect> effect)
  {
    _item_effects.push_back(std::move(effect));
  }

  void addLayoutEffect(std::shared_ptr<Effect> effect)
  {
    _layout_effects.push_back(std::move(effect));
  }

private:
  // this can be static
  void apply_effects(const Effects& effects, const LayoutItem* item, QPainter* p, RenderItemFn r) const {
    if (effects.empty()) {
      r(item, p);
    } else {
      std::ranges::for_each(effects, [&](const auto& e) { e->apply(item, p, r); });
    }
  }

  // this can be static
  void setup_context(QPainter* p, const LayoutItem* item) const
  {
    p->setTransform(item->transform(), true);
  }

  // this can be static
  void render_item(const RenderableItem* item, QPainter* p) const
  {
    // TODO: remove this cast
    if (auto rr = dynamic_pointer_cast<RenderableBase>(item->renderable())) {
      rr->setRenderContext(p);
      rr->render();
    }
  }

  // ... but this one unfortunately not - it must call generic recursive function
  void render_item(const Layout* item, QPainter* p) const
  {
    std::ranges::for_each(item->items(), [this, p](const auto& i) { render_item(i.get(), p); });
  }


  void render_item(const LayoutItem* item, QPainter* p) const
  {
    StateGuard _(p);
    setup_context(p, item);

    if (auto ritem = dynamic_cast<const RenderableItem*>(item)) {
      apply_effects(_item_effects, ritem, p, [this](const LayoutItem* item, QPainter* p) { render_item(static_cast<const RenderableItem*>(item), p); });
    }

    if (auto litem = dynamic_cast<const Layout*>(item)) {
      apply_effects(_layout_effects, litem, p, [this](const LayoutItem* item, QPainter* p) { render_item(static_cast<const Layout*>(item), p); });
    }
  }

private:
  Effects _item_effects;
  Effects _layout_effects;
};
