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

#include "hashable.hpp"

#include <functional>

#include <QPainter>
#include <QRectF>

class Effect : public Hashable {
public:
  virtual ~Effect() = default;

  using RenderItemFn = std::function<void(QPainter*)>;

  // p - output painter
  // r - item rect
  // render_item - function to render original item
  void operator ()(QPainter* p, const QRectF& r,
                   RenderItemFn render_item) const
  {
    apply(p, r, std::move(render_item));
  }

  // effect objects is not supposed to change often
  // but they may have many options what makes hash
  // calculation pretty heavy, so do it only if necessary
  // (i.e. when some of effect options changes)
  // inherited class should care about updating it
  std::size_t hash() const noexcept final { return _hash; }

protected:
  // inherited classes should call this on options change
  void updateHash(std::size_t h) noexcept { _hash = h; }

private:
  virtual void apply(QPainter* p, const QRectF& r,
                     RenderItemFn render_item) const
  {
    Q_UNUSED(r);
    render_item(p);
  }

private:
  std::size_t _hash = Hashable::Invalid;
};


class SimpleEffect : public Effect {
private:
  void apply(QPainter* p, const QRectF& r,
             RenderItemFn render_item) const final
  {
    Q_UNUSED(render_item);
    simpleApply(p, r);
  }

  virtual void simpleApply(QPainter* p, const QRectF& r) const = 0;
};
