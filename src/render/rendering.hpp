/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "layout.hpp"
#include "hashable.hpp"
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
    setInitialGeometry(_res->rect(), _res->advanceX(), _res->advanceY());
  }

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
