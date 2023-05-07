#include "clock_window.hpp"

#include <QContextMenuEvent>
#include <QGridLayout>
#include <QMenu>
#include <QMouseEvent>

#include "app/clock_widget.hpp"
#include "skin/clock_skin.hpp"

struct ClockWindow::impl {
  ClockWidgetWrap* clock_widget;
  QGridLayout* main_layout;
  QMenu* context_menu;
  QPoint drag_pos;
  int snap_threshold = 10;
  bool is_dragging = false;
  bool snap_to_edge = true;
};

ClockWindow::ClockWindow(const SkinPtr& skin, const QDateTime& dt, QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<impl>())
{
  _impl->clock_widget = new ClockWidgetWrap(skin, dt, this);
  _impl->main_layout = new QGridLayout(this);
  _impl->main_layout->addWidget(_impl->clock_widget);

  using namespace Qt::Literals::StringLiterals;
  auto menu = new QMenu(this);
  menu->addAction(QIcon::fromTheme(u"configure"_s), tr("&Settings"),
                  this, &ClockWindow::settingsDialogRequested);
  menu->addSeparator();
  menu->addAction(QIcon::fromTheme(u"help-about"_s), tr("&About"),
                  this, &ClockWindow::aboutDialogRequested);
  menu->addSeparator();
  menu->addAction(QIcon::fromTheme(u"application-exit"_s), tr("&Quit"),
                  this, &ClockWindow::appExitRequested);
  _impl->context_menu = menu;
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

std::shared_ptr<ClockSkin> ClockWindow::skin() const
{
  return _impl->clock_widget->skin();
}

void ClockWindow::setDateTime(const QDateTime& utc)
{
  _impl->clock_widget->setDateTime(utc);
}

void ClockWindow::setTimeZone(const QTimeZone& tz)
{
  _impl->clock_widget->setTimeZone(tz);
}

void ClockWindow::setSeparatorVisible(bool visible)
{
  _impl->clock_widget->setSeparatorVisible(visible);
}

void ClockWindow::setSnapToEdge(bool enable)
{
  _impl->snap_to_edge = enable;
}

void ClockWindow::setSnapThreshold(int threshold)
{
  _impl->snap_threshold = threshold;
}

void ClockWindow::contextMenuEvent(QContextMenuEvent* event)
{
  _impl->context_menu->popup(event->globalPos());
  event->accept();
}

void ClockWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    _impl->drag_pos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    _impl->is_dragging = true;
    event->accept();
  }
}

void ClockWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton) {
    QPoint target_pos = event->globalPosition().toPoint() - _impl->drag_pos;
    if (_impl->snap_to_edge) {
      QRect screen = window()->screen()->availableGeometry();
      QRect widget = frameGeometry();
      if (qAbs(target_pos.x() - screen.left()) <= _impl->snap_threshold)
        target_pos.setX(screen.left());
      if (qAbs(target_pos.y() - screen.top()) <= _impl->snap_threshold)
        target_pos.setY(screen.top());
      if (qAbs(screen.right() - (target_pos.x() + widget.width())) <= _impl->snap_threshold)
        target_pos.setX(screen.right() - widget.width());
      if (qAbs(screen.bottom() - (target_pos.y() + widget.height())) <= _impl->snap_threshold)
        target_pos.setY(screen.bottom() - widget.height());
    }
    move(target_pos);
    event->accept();
  }
}

void ClockWindow::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    _impl->is_dragging = false;
    // TODO: save pos
    event->accept();
  }
}
