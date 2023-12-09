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

#include "skin_resource.hpp"

#include <QFont>

class QCharRenderable final : public SkinResource {
public:
  QCharRenderable(QChar ch, const QFont& font);

  size_t hash() const noexcept override { return _hash; }

  QRectF rect() const override { return _br; }

  qreal advanceX() const override { return _ax; }
  qreal advanceY() const override { return _ay; }

  void render(QPainter* p) override;

private:
  QChar _ch;
  QRectF _br;
  qreal _ax;
  qreal _ay;
  const QFont& _font;
  size_t _hash;
};
