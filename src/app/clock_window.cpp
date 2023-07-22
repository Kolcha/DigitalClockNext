#include "clock_window.hpp"

#include <QContextMenuEvent>
#include <QGridLayout>
#include <QMenu>
#include <QMouseEvent>

#include "app/clock_widget.hpp"
#include "app/window_positioning.hpp"
#include "skin/clock_skin.hpp"

struct ClockWindow::impl {
  StatePtr state;
  ClockWidgetWrap* clock_widget;
  QGridLayout* main_layout;
  QMenu* context_menu;
  QPoint drag_pos;
  QPoint ref_point;
  Qt::Alignment alignment = Qt::AlignTop | Qt::AlignLeft;
  int snap_threshold = 10;
  bool is_dragging = false;
  bool snap_to_edge = true;
  bool separator_flashes = true;
};

ClockWindow::State::~State() = default;

ClockWindow::ClockWindow(const SkinPtr& skin, const QDateTime& dt, StatePtr state, QWidget* parent)
  : QWidget(parent)
  , _impl(std::make_unique<impl>())
{
  Q_ASSERT(state);
  _impl->state = std::move(state);
  _impl->clock_widget = new ClockWidgetWrap(skin, dt, this);
  // clock widget supports resize and fills all available space by default
  _impl->clock_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  _impl->main_layout = new QGridLayout(this);
  _impl->main_layout->addWidget(_impl->clock_widget);
  _impl->main_layout->setSizeConstraint(QLayout::SetFixedSize);

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

  // even for debug purpose, window must be frameless,
  // at least on X11, see Qt documentation for details
  setWindowFlag(Qt::FramelessWindowHint);

  // restore window state
  _impl->ref_point = _impl->state->getPos();
  _impl->alignment = _impl->state->getAlignment();
}

ClockWindow::~ClockWindow() = default;

bool ClockWindow::isSeparatorVisible() const
{
  return _impl->clock_widget->isSeparatorVisible();
}

bool ClockWindow::isSeparatorFlashes() const
{
  return _impl->separator_flashes;
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

void ClockWindow::setSeparatorFlashes(bool flashes)
{
  _impl->separator_flashes = flashes;
}

void ClockWindow::flipSeparator()
{
  if (!_impl->separator_flashes)
    return;

  setSeparatorVisible(!isSeparatorVisible());
}

void ClockWindow::scale(int sx, int sy)
{
  _impl->clock_widget->scale(sx / 100., sy / 100.);
}

void ClockWindow::setAlignment(Qt::Alignment alignment)
{
  if (_impl->alignment == alignment)
    return;

  _impl->alignment = alignment;
  _impl->ref_point = pick_ref_point({pos(), sizeHint()}, alignment);
  _impl->state->setAlignment(alignment);
  _impl->state->setPos(_impl->ref_point);
}

void ClockWindow::setSnapToEdge(bool enable)
{
  _impl->snap_to_edge = enable;
}

void ClockWindow::setSnapThreshold(int threshold)
{
  _impl->snap_threshold = threshold;
}

void ClockWindow::skinConfigured()
{
  _impl->clock_widget->skinConfigured();
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
    _impl->ref_point = pick_ref_point(frameGeometry(), _impl->alignment);
    _impl->state->setPos(_impl->ref_point);
    event->accept();
  }
}

void ClockWindow::resizeEvent(QResizeEvent* event)
{
  // ignore the first resize event, it happens before show()
  // also ignore invalid (yes, this happens!) frame geometry
  if (!_impl->is_dragging && event->oldSize().isValid() && frameGeometry().isValid())
    move(aligned_rect(frameGeometry(), _impl->ref_point, _impl->alignment).topLeft());
  QWidget::resizeEvent(event);
}

void ClockWindow::showEvent(QShowEvent* event)
{
  // frame geometry is known before showing
  if (!event->spontaneous())
    move(aligned_rect(frameGeometry(), _impl->ref_point, _impl->alignment).topLeft());
  QWidget::showEvent(event);
}
