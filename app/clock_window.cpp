#include "clock_window.hpp"

#include <QGridLayout>

#include "app/clock_widget.hpp"
#include "skin/clock_skin.hpp"

struct ClockWindow::impl {
  ClockWidgetWrap* clock_widget;
  QGridLayout* main_layout;
  QDateTime utc_time;
  QTimeZone time_zone;
};

ClockWindow::ClockWindow(const SkinPtr& skin, const QDateTime& dt, QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<impl>())
{
  _impl->utc_time = dt.toUTC();
  _impl->time_zone = dt.timeZone();
  _impl->clock_widget = new ClockWidgetWrap(skin, dt, this);
  _impl->main_layout = new QGridLayout(this);
  _impl->main_layout->addWidget(_impl->clock_widget);
}

ClockWindow::~ClockWindow() = default;

bool ClockWindow::isSeparatorVisible() const
{
  return _impl->clock_widget->isSeparatorVisible();
}

void ClockWindow::setSkin(std::shared_ptr<ClockSkin> skin)
{
  _impl->clock_widget->setSkin(std::move(skin));
}

void ClockWindow::setDateTime(const QDateTime& utc)
{
  _impl->clock_widget->setDateTime(utc.toTimeZone(_impl->time_zone));
  _impl->utc_time = utc;
}

void ClockWindow::setTimeZone(const QTimeZone& tz)
{
  _impl->clock_widget->setDateTime(_impl->utc_time.toTimeZone(tz));
  _impl->time_zone = tz;
}

void ClockWindow::setSeparatorVisible(bool visible)
{
  _impl->clock_widget->setSeparatorVisible(visible);
}
