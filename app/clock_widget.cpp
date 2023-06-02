#include "clock_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include "clock/clock_widget.hpp"

struct ClockWidgetWrap::impl {
  std::unique_ptr<ClockWidget> n_impl;
  std::shared_ptr<ClockSkin> skin;
  QDateTime dt;
  QTimeZone tz;
  bool seps_visible = true;

  impl(const QDateTime& dt, const std::shared_ptr<ClockSkin>& skin)
    : n_impl(std::make_unique<ClockWidget>(dt, skin))
    , skin(skin)
    , dt(dt.toUTC())
    , tz(dt.timeZone())
  {}
};

ClockWidgetWrap::ClockWidgetWrap(const SkinPtr& skin, const QDateTime& dt, QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<impl>(dt, skin))
{
}

ClockWidgetWrap::~ClockWidgetWrap() = default;

QSize ClockWidgetWrap::sizeHint() const
{
  return minimumSizeHint();
}

QSize ClockWidgetWrap::minimumSizeHint() const
{
  return _impl->n_impl->geometry().size().toSize();
}

bool ClockWidgetWrap::isSeparatorVisible() const
{
  return _impl->n_impl->isSeparatorVisible();
}

void ClockWidgetWrap::setSkin(std::shared_ptr<ClockSkin> skin)
{
  _impl->n_impl = std::make_unique<ClockWidget>(_impl->dt, skin);
  _impl->skin = std::move(skin);
}

std::shared_ptr<ClockSkin> ClockWidgetWrap::skin() const
{
  return _impl->skin;
}

void ClockWidgetWrap::setDateTime(const QDateTime& dt)
{
  _impl->dt = dt.toUTC();
  _impl->n_impl->setDateTime(dt.toTimeZone(_impl->tz));
  updateGeometry();
  update();
}

void ClockWidgetWrap::setTimeZone(const QTimeZone& tz)
{
  _impl->tz = tz;
  setDateTime(_impl->dt);
}

void ClockWidgetWrap::setSeparatorVisible(bool visible)
{
  _impl->seps_visible = visible;
  update();
}

void ClockWidgetWrap::paintEvent(QPaintEvent* event)
{
  _impl->n_impl->setSeparatorVisible(_impl->seps_visible);
  QPainter p(this);
  _impl->n_impl->render(&p);
  event->accept();
}
