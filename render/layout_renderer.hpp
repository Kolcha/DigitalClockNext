#pragma once

#include <ranges>

#include <QPainter>

#include "core/layout.hpp"
#include "core/renderable_item.hpp"
#include "render/state_guard.hpp"

class LayoutRenderer {
public:
  void render(const LayoutItem* item, QPainter* p) const
  {
    render_item(item, p);
  }

private:
  static void setup_context(QPainter* p, const LayoutItem* item)
  {
    p->setTransform(item->transform(), true);
  }

  static void render_item(const RenderableItem* item, QPainter* p)
  {
    item->renderable()->render(p);
  }

  static void render_item(const Layout* item, QPainter* p)
  {
    std::ranges::for_each(item->items(), [p](const auto& i) { render_item(i.get(), p); });
  }

  static void render_item(const LayoutItem* item, QPainter* p)
  {
    StateGuard _(p);
    setup_context(p, item);

    if (auto ritem = dynamic_cast<const RenderableItem*>(item)) {
      render_item(ritem, p);
    }

    if (auto litem = dynamic_cast<const Layout*>(item)) {
      render_item(litem, p);
    }
  }
};
