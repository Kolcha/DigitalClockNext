/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "clock_widget.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QPixmapCache>

#include "skin.hpp"

class ClockWidgetImpl : public SkinObserver,
                        public std::enable_shared_from_this<ClockWidgetImpl> {
public:
  ClockWidgetImpl(QWidget* w, const QDateTime& dt)
      : _widget(w)
      , _dt(dt.toUTC())
      , _tz(dt.timeZone())
      , _last_palette(w->palette())
  {
    Q_ASSERT(_widget);
  }

  void setSkin(std::shared_ptr<Skin> skin)
  {
    _skin = std::move(skin);
    if (_skin) _skin->addObserver(weak_from_this());
    _glyph.reset();
    update();
  }

  std::shared_ptr<Skin> skin() const { return _skin; }

  void setDateTime(const QDateTime& dt)
  {
    _dt = dt.toUTC();
    update();
  }

  void setTimeZone(const QTimeZone& tz)
  {
    _tz = tz;
    update();
  }

  void animateSeparator()
  {
    if (!_skin) return;
    _skin->animateSeparator();
    update();
  }

  void scale(qreal kx, qreal ky)
  {
    _kx = std::clamp(kx, 0.01, 10.0);
    _ky = std::clamp(ky, 0.01, 10.0);
    update();
  }

  QSizeF size() const
  {
    if (!_glyph) return {400., 150.};
    auto s = _glyph->rect().size();
    return {_kx * s.width(), _ky * s.height()};
  }

  void draw(QPainter* p)
  {
    // skin may use system palette and cache rendering results,
    // so drop cache on system theme change (e.g. light/dark)
    if (_widget->palette() != _last_palette) {
      _last_palette = _widget->palette();
      QPixmapCache::clear();
    }
    if (!_glyph) return;
    p->setRenderHint(QPainter::Antialiasing);
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->scale(_kx, _ky);
    p->translate(-_glyph->rect().topLeft());
    _glyph->draw(p);
  }

  void onConfigurationChanged() override { update(); }

private:
  void update()
  {
    if (!_skin) return;
    _glyph = _skin->process(_dt.toTimeZone(_tz));
    _widget->updateGeometry();
    _widget->update();
  }

private:
  QWidget* _widget;
  std::shared_ptr<Skin> _skin;
  std::shared_ptr<Resource> _glyph;
  QDateTime _dt;
  QTimeZone _tz;
  qreal _kx = 1;
  qreal _ky = 1;
  QPalette _last_palette;   // used just to detect theme changes
};


struct ClockWidget::impl {
  impl(QWidget* w)
      : d(std::make_shared<ClockWidgetImpl>(w, QDateTime::currentDateTime()))
  {}

  std::shared_ptr<ClockWidgetImpl> d;
};


ClockWidget::ClockWidget(QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<impl>(this))
{
}

ClockWidget::~ClockWidget() = default;

QSize ClockWidget::sizeHint() const
{
  return minimumSizeHint();
}

QSize ClockWidget::minimumSizeHint() const
{
  return _impl->d->size().toSize();
}

void ClockWidget::setSkin(std::shared_ptr<Skin> skin)
{
  _impl->d->setSkin(std::move(skin));
}

std::shared_ptr<Skin> ClockWidget::skin() const
{
  return _impl->d->skin();
}

void ClockWidget::setDateTime(const QDateTime& dt)
{
  _impl->d->setDateTime(dt);
}

void ClockWidget::setTimeZone(const QTimeZone& tz)
{
  _impl->d->setTimeZone(tz);
}

void ClockWidget::animateSeparator()
{
  _impl->d->animateSeparator();
}

void ClockWidget::scale(qreal kx, qreal ky)
{
  _impl->d->scale(kx, ky);
}

void ClockWidget::paintEvent(QPaintEvent* event)
{
  QPainter p(this);
  _impl->d->draw(&p);
  event->accept();
}
