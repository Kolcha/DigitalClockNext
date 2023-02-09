#pragma once

#include <functional>
#include <memory>
#include <ranges>
#include <vector>

#include <QPainter>

#include "core/layout.hpp"
#include "core/renderable_item.hpp"
#include "render/state_guard.hpp"

/*
 * void apply_effects(effects, item, ctx, render_item);
 *
 * problem: render_item() has different signatures
 * options:
 *  - use templates     cons: multiple implementations
 *  - std::function     cons: lambda with cast inside
 */

class Effect {
};


class LayoutRenderer {
public:
  void render(const LayoutItem* item, QPainter* p) const
  {
    // calls recursive function
    render_item(item, p);
  }

  // TODO: add effects API

private:
  void apply_effects(const LayoutItem* item, QPainter* p,
                     std::function<void(const LayoutItem*, QPainter*)> r) const {
    // TODO: apply effects
    // assumes flow:
    // - effect context setup
    // - item rendering
    // - cleanup
    r(item, p);
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
      apply_effects(ritem, p, [this](const LayoutItem* item, QPainter* p) { render_item(static_cast<const RenderableItem*>(item), p); });
    }

    if (auto litem = dynamic_cast<const Layout*>(item)) {
      apply_effects(litem, p, [this](const LayoutItem* item, QPainter* p) { render_item(static_cast<const Layout*>(item), p); });
    }
  }

  std::vector<std::shared_ptr<Effect>> _effects;
};
