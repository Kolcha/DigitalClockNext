#include "composite.hpp"

#include <QPainter>
#include <QPixmap>

#include "core/hasher.hpp"
#include "render/state_guard_qpainter.hpp"

void CompositeEffect::addEffect(std::shared_ptr<Effect> effect)
{
  _effects.push_back(std::move(effect));
  updateHash(hasher(_effects));
}

void CompositeEffect::clearEffects()
{
  _effects.clear();
  updateHash(hasher(_effects));
}

void CompositeEffect::apply(QPainter* p, const QRectF& r, RenderItemFn render_item) const
{
  StateGuard _(p);
  QPixmap buffer(p->device()->width(), p->device()->height());
  buffer.setDevicePixelRatio(p->device()->devicePixelRatioF());
  buffer.fill(Qt::transparent);
  {
    QPainter pp(&buffer);
    pp.setRenderHint(QPainter::Antialiasing);
    pp.setTransform(p->transform());
    for (const auto& effect : _effects)
      (*effect)(&pp, r, render_item);
  }
  p->resetTransform();
  p->drawPixmap(0, 0, buffer);
}
