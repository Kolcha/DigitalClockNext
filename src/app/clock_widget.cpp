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

#include "clock_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include "skin.hpp"

struct ClockWidgetWrap::impl {
  std::shared_ptr<Skin> skin;
  std::shared_ptr<Glyph> item;
  QDateTime dt;
  QTimeZone tz;
  qreal kx = 1;
  qreal ky = 1;

  impl(const QDateTime& dt, const std::shared_ptr<Skin>& skin)
    : skin(skin)
    , dt(dt.toUTC())
    , tz(dt.timeZone())
  {
  }

  QSizeF scaledSize() const noexcept
  {
    if (!item) return {400., 150.};
    auto s = item->geometry().size();
    return {kx * s.width(), ky * s.height()};
  }
};

ClockWidgetWrap::ClockWidgetWrap(QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<impl>(QDateTime::currentDateTime(), nullptr))
{
}

ClockWidgetWrap::~ClockWidgetWrap() = default;

QSize ClockWidgetWrap::sizeHint() const
{
  return minimumSizeHint();
}

QSize ClockWidgetWrap::minimumSizeHint() const
{
  return _impl->scaledSize().toSize();
}

void ClockWidgetWrap::setSkin(std::shared_ptr<Skin> skin)
{
  _impl->skin = std::move(skin);
  _impl->item.reset();
  skinConfigured();
}

std::shared_ptr<Skin> ClockWidgetWrap::skin() const
{
  return _impl->skin;
}

void ClockWidgetWrap::setDateTime(const QDateTime& dt)
{
  _impl->dt = dt.toUTC();
  if (!_impl->skin) return;
  _impl->item = _impl->skin->process(dt.toTimeZone(_impl->tz));
  updateGeometry();
  update();
}

void ClockWidgetWrap::setTimeZone(const QTimeZone& tz)
{
  _impl->tz = tz;
  setDateTime(_impl->dt);
}

void ClockWidgetWrap::animateSeparator()
{
  if (!_impl->skin) return;
  _impl->skin->animateSeparator();
  update();
}

void ClockWidgetWrap::scale(qreal kx, qreal ky)
{
  _impl->kx = kx;
  _impl->ky = ky;
  updateGeometry();
  update();
}

void ClockWidgetWrap::skinConfigured()
{
  if (!_impl->skin) return;
  // skin properties may vary depending on skin type and
  // they are unknown here and may be modified outside
  // we should be notified about these changes to rebuild
  // widget's geometry after skin properties modification
  _impl->item = _impl->skin->process(_impl->dt.toTimeZone(_impl->tz));
  updateGeometry();
  update();
}

void ClockWidgetWrap::paintEvent(QPaintEvent* event)
{
  if (!_impl->item) return;
  QPainter p(this);
  auto s = _impl->item->geometry().size();
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.scale(width() / s.width(), height() / s.height());
  p.translate(-_impl->item->geometry().topLeft());
  _impl->item->draw(&p);
  event->accept();
}
