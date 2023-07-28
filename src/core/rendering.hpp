#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <QPainter>
#include <QPixmap>

#include "effect.hpp"
#include "geometry.hpp"
#include "skin_resource.hpp"

class SkinElement {
public:
  virtual ~SkinElement() = default;

  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

  void setVisible(bool visible) noexcept { _visible = visible; }
  bool isVisible() const noexcept { return _visible; }

  // TODO: make protected ?
  virtual std::shared_ptr<LayoutItem> item() = 0;

  void render(QPainter* p)
  {
    if (!isVisible() || item()->rect().isEmpty())
      return;

    p->save();
    p->setTransform(item()->transform(), true);

    // rasterization for effects and efficient caching
    auto br = p->transform().mapRect(item()->rect());
    QPixmap pxm((br.size() * p->device()->devicePixelRatioF()).toSize());
    pxm.setDevicePixelRatio(p->device()->devicePixelRatioF());
    pxm.fill(Qt::transparent);
    {
      QPainter pp(&pxm);
      pp.setRenderHint(QPainter::Antialiasing);
      pp.translate(-br.topLeft());
      pp.setTransform(p->transform(), true);
      _effects.empty() ? renderImpl(&pp) : applyEffects(&pp);
    }

    // TODO: cache rendering result
//    pxm.save(QString("%1.png").arg((uintptr_t)(this)));

//    renderImpl(p);
    p->resetTransform();
    p->translate(br.topLeft());
    p->drawPixmap(0, 0, pxm);
    p->restore();
  }

protected:
  virtual void renderImpl(QPainter* p) = 0;

private:
  void applyEffects(QPainter* p)
  {
    auto render_item = [this](QPainter* p) { renderImpl(p); };
    for (const auto& effect : _effects)
      (*effect)(p, item()->rect(), render_item);
  }

private:
  std::vector<std::shared_ptr<Effect>> _effects;
  bool _visible = true;
};


class SimpleSkinElement : public LayoutItem, public SkinElement {
public:
  explicit SimpleSkinElement(std::shared_ptr<SkinResource> res)
    : _res(std::move(res))
  {
    assert(_res);
    setRect(_res->rect());
  }

  qreal advanceX() const override { return _res->advanceX(); }
  qreal advanceY() const override { return _res->advanceY(); }

  std::shared_ptr<LayoutItem> item() override { return shared_from_this(); }

protected:
  void renderImpl(QPainter* p) override { _res->render(p); }

private:
  std::shared_ptr<SkinResource> _res;
};


class LayoutSkinElement : public Layout, public SkinElement {
public:
  void addElement(std::shared_ptr<SkinElement> element)
  {
    addItem(element->item()); // TODO: make protected?
    _elements.push_back(std::move(element));
  }

  std::shared_ptr<LayoutItem> item() override { return shared_from_this(); }

protected:
  void renderImpl(QPainter* p) override
  {
    std::ranges::for_each(_elements, [p](const auto& e) { e->render(p); });
  }

private:
  std::vector<std::shared_ptr<SkinElement>> _elements;
};
