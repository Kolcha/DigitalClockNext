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

#include "effect.hpp"

#include <memory>
#include <vector>

class CompositeEffect final : public Effect {
public:
  void addEffect(std::shared_ptr<Effect> effect);
  void clearEffects();

  const auto& effects() const noexcept { return _effects; }

private:
  void apply(QPainter* p, const QRectF& r,
             RenderItemFn render_item) const override;

private:
  std::vector<std::shared_ptr<Effect>> _effects;
};
