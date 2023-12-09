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

#include "renderable_factory.hpp"

#include <QFont>

#include "char_renderable.hpp"

class QCharRenderableFactory final : public RenderableFactory {
public:
  explicit QCharRenderableFactory(const QFont& fnt)
    : _fnt(fnt)
  {}

public:
  const QFont& font() const noexcept { return _fnt; }

  std::shared_ptr<SkinResource> item(QChar ch) const override
  {
    return std::make_shared<QCharRenderable>(ch, _fnt);
  }

private:
  QFont _fnt;
};
