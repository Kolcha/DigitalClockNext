#pragma once

#include "core/effect.hpp"

#include <memory>
#include <vector>

#include "render/state_guard_qpainter.hpp"

class CompositeEffect final : public Effect {
  void apply(QPainter* p, const QRectF& r, RenderItemFn render) const override
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
        (*effect)(&pp, r, render);
    }
    p->resetTransform();
    p->drawPixmap(0, 0, buffer);
  }

public:
  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

  void clearEffects() noexcept
  {
    _effects.clear();
  }

  const auto& effects() const noexcept { return _effects; }

private:
  std::vector<std::shared_ptr<Effect>> _effects;
};
