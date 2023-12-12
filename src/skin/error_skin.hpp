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

#include "skin.hpp"
#include "layout.hpp"
#include "resource.hpp"

/**
 * @brief Error message implementation
 *
 * For now error message text is hardcaoded and assumes only
 * skin has not been loaded situation.
 *
 * Message text is in English only, not translatable.
 */
class ErrorMessage final : public Resource {
public:
  QRectF rect() const noexcept override { return {0, 0, 400, 150}; }

  qreal advanceX() const noexcept override { return rect().width(); }
  qreal advanceY() const noexcept override { return rect().height(); }

  void draw(QPainter* p) override;

  size_t cacheKey() const noexcept override { return qHash(_visible); }

  void setVisible(bool visible) noexcept { _visible = visible; }
  bool isVisible() const noexcept { return _visible; }

private:
  bool _visible = true;
};

/**
 * @brief Simple skin to display error message
 *
 * Can be used as "placeholder" in case of requested skin has not been loaded.
 */
class ErrorSkin final : public Skin {
public:
  ErrorSkin()
    : _msg(std::make_shared<ErrorMessage>())
    , _renderable(std::make_shared<SimpleGlyph>(_msg))
  {}

  std::shared_ptr<Glyph> process(const QDateTime& dt) noexcept override
  {
    Q_UNUSED(dt);
    return _renderable;
  }

  void setSeparatorAnimationEnabled([[maybe_unused]] bool enabled) override {}
  void animateSeparator() override { _msg->setVisible(!_msg->isVisible()); }

private:
  std::shared_ptr<ErrorMessage> _msg;
  std::shared_ptr<Glyph> _renderable;
};
