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


class CompositeEffect final : public Effect {
public:
  void apply(const LayoutItem* item, QPainter* p, RenderItemFn r) const override
  {
    if (_effects.empty() || item->rect().size().isEmpty()) {
      r(item, p);
    } else {
      QSizeF sz = item->rect().size() * p->device()->devicePixelRatioF();
      QPixmap res(sz.toSize());
      res.setDevicePixelRatio(p->device()->devicePixelRatioF());
      res.fill(Qt::transparent);
      for (const auto& e : _effects) {
        QPainter ep(&res);
        ep.translate(-item->rect().topLeft());
        e->apply(item, &ep, r);
      }
      p->drawPixmap(item->rect(), res, res.rect());
    }
  }

  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

private:
  std::vector<std::shared_ptr<Effect>> _effects;
};


class LayoutRenderer {
public:
  LayoutRenderer()
    : _item_effect(std::make_shared<CompositeEffect>())
    , _layout_effect(std::make_shared<CompositeEffect>())
  {}

  void render(const LayoutItem* item, QPainter* p) const
  {
    // calls recursive function
    render_item(item, p);
  }

  void addItemEffect(std::shared_ptr<Effect> effect)
  {
    _item_effect->addEffect(std::move(effect));
  }

  void addLayoutEffect(std::shared_ptr<Effect> effect)
  {
    _layout_effect->addEffect(std::move(effect));
  }

private:
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
      _item_effect->apply(ritem, p, [this](const LayoutItem* item, QPainter* p) { render_item(static_cast<const RenderableItem*>(item), p); });
    }

    if (auto litem = dynamic_cast<const Layout*>(item)) {
      _layout_effect->apply(litem, p, [this](const LayoutItem* item, QPainter* p) { render_item(static_cast<const Layout*>(item), p); });
    }
  }

private:
  std::shared_ptr<CompositeEffect> _item_effect;
  std::shared_ptr<CompositeEffect> _layout_effect;
};
