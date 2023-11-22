#include "clock_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include "clock/clock_widget.hpp"

struct ClockWidgetWrap::impl {
  std::unique_ptr<ClockWidget> n_impl;
  std::shared_ptr<ClockSkin> skin;
  QDateTime dt;
  QTimeZone tz;
  qreal kx = 1;
  qreal ky = 1;
  bool seps_visible = true;

  impl(const QDateTime& dt, const std::shared_ptr<ClockSkin>& skin)
    : skin(skin)
    , dt(dt.toUTC())
    , tz(dt.timeZone())
  {
    if (skin)
      n_impl = std::make_unique<ClockWidget>(dt, skin);
  }

  QSizeF scaledSize() const noexcept
  {
    if (!n_impl) return {400., 150.};
    auto s = n_impl->geometry().size();
    return {kx * s.width(), ky * s.height()};
  }
};

ClockWidgetWrap::ClockWidgetWrap(QWidget* parent)
  : ClockWidgetWrap(nullptr, QDateTime::currentDateTime(), parent)
{
}

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
  return _impl->scaledSize().toSize();
}

void ClockWidgetWrap::setSkin(std::shared_ptr<ClockSkin> skin)
{
  if (skin)
    _impl->n_impl = std::make_unique<ClockWidget>(_impl->dt.toTimeZone(_impl->tz), skin);
  else
    _impl->n_impl.reset();
  _impl->skin = std::move(skin);
  updateGeometry();
  update();
}

std::shared_ptr<ClockSkin> ClockWidgetWrap::skin() const
{
  return _impl->skin;
}

void ClockWidgetWrap::setDateTime(const QDateTime& dt)
{
  _impl->dt = dt.toUTC();
  if (!_impl->n_impl) return;
  _impl->n_impl->setDateTime(dt.toTimeZone(_impl->tz));
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
  _impl->seps_visible = !_impl->seps_visible; // for error animation
  if (!_impl->n_impl) return;
  _impl->n_impl->animateSeparator();
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
  if (!_impl->n_impl) return;
  // skin properties may vary depending on skin type and
  // they are unknown here and may be modified outside
  // we should be notified about these changes to rebuild
  // widget's geometry after skin properties modification
  _impl->n_impl->setDateTime(_impl->dt.toTimeZone(_impl->tz));
  updateGeometry();
  update();
}

void ClockWidgetWrap::paintEvent(QPaintEvent* event)
{
  QPainter p(this);
  if (Q_UNLIKELY(!_impl->n_impl)) {
    // draw nice error message instead of just ignoring paint event
    using namespace Qt::Literals::StringLiterals;
    p.fillRect(rect(), Qt::black);
    auto fnt = font();
    fnt.setPointSize(14);
    p.setFont(fnt);
    p.setPen(QColor(112, 96, 240));
    auto r = QRectF(x(), y(), width(), height() / 4.);
    p.drawText(r, Qt::AlignCenter, u"Digital Clock Next"_s);
    if (_impl->seps_visible) {
      p.setPen(Qt::red);
      fnt.setPointSize(48);
      p.setFont(fnt);
      p.drawText(rect(), Qt::AlignCenter, u"ERROR"_s);
    }
    p.setPen(Qt::white);
    fnt.setPointSize(12);
    p.setFont(fnt);
    r.translate(0, 3 * r.height());
    p.drawText(r, Qt::AlignCenter, u"skin has not been loaded"_s);
    return;
  }
  auto s = _impl->n_impl->geometry().size();
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.scale(width() / s.width(), height() / s.height());
  _impl->n_impl->render(&p);
  event->accept();
}
