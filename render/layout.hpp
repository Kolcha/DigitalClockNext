#pragma once

#include <ranges>

#include <QPainter>

#include "core/layout.hpp"
#include "core/renderable.hpp"
#include "core/renderable_item.hpp"

#include "state_guard.hpp"

// WIP, just a set of functions moved to own file, even not inline for now
// TODO: create some class/entity for that and hide implementation details

void setup_context(QPainter* p, const LayoutItem* item)
{
  p->setTransform(item->transform(), true);
}

// TODO: maybe implement using templates specialization instead of overload?
// generic recursive function declaration
void render_item(const LayoutItem* item, QPainter* p);

void render_item(const RenderableItem* item, QPainter* p)
{
  // TODO: remove this cast
  if (auto rr = dynamic_pointer_cast<RenderableBase>(item->renderable())) {
    rr->setRenderContext(p);
    rr->render();
  }
}

void render_item(const Layout* item, QPainter* p)
{
  std::ranges::for_each(item->items(), [p](const auto& i) { render_item(i.get(), p); });
}


void render_item(const LayoutItem* item, QPainter* p)
{
  StateGuard _(p);
  setup_context(p, item);

  if (auto ritem = dynamic_cast<const RenderableItem*>(item)) {
    // TODO: consider "effects" (aka texturing) injection here
    render_item(ritem, p);
  }

  if (auto litem = dynamic_cast<const Layout*>(item)) {
    // TODO: consider "effects" (aka texturing) injection here
    render_item(litem, p);
  }
}
