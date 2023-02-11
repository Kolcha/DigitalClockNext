#pragma once

#include "core/effect.hpp"

#include <memory>
#include <vector>

#include <QPainter>
#include <QPixmap>

#include "core/renderable.hpp"

class CompositeEffect final : public Effect {
public:
  void apply(const Renderable* item, QPainter* p, RenderItemFn r) const override
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
