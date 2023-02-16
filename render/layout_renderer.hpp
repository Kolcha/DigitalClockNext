#pragma once

#include <assert.h>
#include <ranges>

#include <QPainter>
#include <QPixmap>

#include "core/effect.hpp"
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

  static void apply_effects(const RenderableItem* item, QPainter* p)
  {
    assert(!item->rect().isEmpty());
    assert(!item->effects().empty());
    StateGuard _(p);
    qreal sx = p->transform().m11();
    qreal sy = p->transform().m22();
    QSizeF s = item->rect().size() * p->device()->devicePixelRatioF();
    s.setWidth(s.width() * sx);
    s.setHeight(s.height() * sy);
    QPixmap res(s.toSize());
    res.setDevicePixelRatio(p->device()->devicePixelRatioF());
    res.fill(Qt::transparent);
    for (const auto& effect : item->effects()) {
      QPainter ep(&res);
      effect->apply(item->renderable().get(), &ep, [=](const Renderable* r, QPainter* p) {
        StateGuard _(p);
        p->scale(sx, sy);
        p->translate(-r->rect().topLeft());
        r->render(p);
      });
    }
    QTransform t = p->transform();
    t.setMatrix(1.0, t.m12(), t.m13(), t.m21(), 1.0, t.m23(), t.m31(), t.m32(), t.m33());
    p->setTransform(t);
    p->drawPixmap(QPointF(sx * item->rect().x(), sy * item->rect().y()), res);
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
      render_item(ritem, p);
    }

    if (auto litem = dynamic_cast<const Layout*>(item)) {
      render_item(litem, p);
    }
  }
};
