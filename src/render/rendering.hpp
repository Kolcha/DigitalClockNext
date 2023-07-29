#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "core/geometry.hpp"
#include "core/hashable.hpp"
#include "effect.hpp"
#include "skin_resource.hpp"

class SkinElement : public Hashable {
public:
  virtual ~SkinElement() = default;

  void addEffect(std::shared_ptr<Effect> effect)
  {
    _effects.push_back(std::move(effect));
  }

  const auto& effects() const noexcept { return _effects; }

  void setVisible(bool visible) noexcept { _visible = visible; }
  bool isVisible() const noexcept { return _visible; }

  virtual std::shared_ptr<LayoutItem> item() = 0;

  void render(QPainter* p);

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

  size_t hash() const noexcept override;

protected:
  void renderImpl(QPainter* p) override { _res->render(p); }

private:
  std::shared_ptr<SkinResource> _res;
};


class LayoutSkinElement : public Layout, public SkinElement {
public:
  void addElement(std::shared_ptr<SkinElement> element)
  {
    addItem(element->item());
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
