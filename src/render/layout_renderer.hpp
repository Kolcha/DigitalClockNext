#pragma once

#include <assert.h>
#include <ranges>

#include <QPainter>
#include <QPixmap>

#include "core/effect.hpp"
#include "core/layout.hpp"
#include "core/renderable_item.hpp"
#include "render/state_guard_qpainter.hpp"

class LayoutRenderer {
public:
  void render(const LayoutItem* item, QPainter* p) const
  {
    render_item(item, p);
  }

private:
  static void setup_context(QPainter* p, const LayoutItem* item)
  {
    p->translate(item->pos());
  }

  static void setup_context(QPainter* p, const RenderableItem* item)
  {
    p->setTransform(item->transform(), true);
  }

  static void apply_effects(const RenderableItem* item, QPainter* p)
  {
    assert(!item->rect().isEmpty());
    assert(!item->effects().empty());

    QTransform t = p->transform();
    QRectF br = p->transform().map(QPolygonF(item->rect())).boundingRect();

    QSizeF sz = br.size() * p->device()->devicePixelRatioF();
    QPixmap pxm(sz.toSize());
    pxm.setDevicePixelRatio(p->device()->devicePixelRatioF());
    pxm.fill(Qt::transparent);

    // TODO: consider pixmap interface: QPixmap effect->apply(prev_res, orig_image);
    for (const auto& effect : item->effects()) {
      QPainter ep(&pxm);
      effect->apply(item->renderable().get(), &ep, [&](const Renderable* r, QPainter* p) {
        StateGuard _(p);
        p->translate(-br.topLeft());
        p->setTransform(t, true);
        r->render(p);
      });
    }

    p->resetTransform();
    p->drawPixmap(br.topLeft(), pxm);
  }

  static void render_item(const RenderableItem* item, QPainter* p)
  {
    if (item->rect().isEmpty())
      return;

    if (!item->effects().empty())
      apply_effects(item, p);
    else
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
      setup_context(p, ritem);
      render_item(ritem, p);
    }

    if (auto litem = dynamic_cast<const Layout*>(item)) {
      render_item(litem, p);
    }
  }
};
