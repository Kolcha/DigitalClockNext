#include "clock_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include "clock/clock_widget.hpp"

ClockWidgetWrap::ClockWidgetWrap(const SkinPtr& skin, const QDateTime& dt, QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<ClockWidget>(dt, skin))
{
}

ClockWidgetWrap::~ClockWidgetWrap() = default;

QSize ClockWidgetWrap::sizeHint() const
{
  return minimumSizeHint();
}

QSize ClockWidgetWrap::minimumSizeHint() const
{
  return _impl->geometry().size().toSize();
}

bool ClockWidgetWrap::isSeparatorVisible() const
{
  return _impl->isSeparatorVisible();
}

void ClockWidgetWrap::setSkin(std::shared_ptr<ClockSkin> skin)
{
  // TODO: implement
}

void ClockWidgetWrap::setDateTime(const QDateTime& dt)
{
  _impl->setDateTime(dt);
  updateGeometry();
  update();
}

void ClockWidgetWrap::setSeparatorVisible(bool visible)
{
  _impl->setSeparatorVisible(visible);
}

void ClockWidgetWrap::paintEvent(QPaintEvent* event)
{
  QPainter p(this);
  _impl->render(&p);
  event->accept();
}
