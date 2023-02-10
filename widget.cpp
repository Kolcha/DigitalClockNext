#include "widget.hpp"

#include <chrono>
#include <memory>
#include <ranges>

#include <QDebug>
#include <QDateTime>
#include <QFontMetrics>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QTimer>
#include <QConicalGradient>
#include <QLinearGradient>

#include "core/layout_builder.hpp"
#include "core/linear_layout.hpp"
#include "render/state_guard.hpp"
#include "skin/char_renderable_factory.hpp"
#include "skin/classic_skin.hpp"
#include "clock/clock_widget.hpp"
#include "render/texturing_effect.hpp"


// TODO: move to skin implementaion file
// what if several implementations use LinearLayout?
// let's keep here at least for now
template<>
void LayoutBuilder<LinearLayout>::LayoutTypeProxy::build()
{
  LinearLayout::rebuildLayout();
}

// TODO: move to layout renderer implementation file?
// very likely no - template must be defined before specialization
// and those particular specializations are widely used
template<>
void StateGuard<QPainter>::save(QPainter* p) { p->save(); }
template<>
void StateGuard<QPainter>::restore(QPainter* p) { p->restore(); }


Widget::Widget(QWidget *parent)
  : QWidget(parent)
{
  _timer = new QTimer(this);

  QFont fnt(u"Bad Script"_qs, 72);
//  QFont fnt(u"Segoe Script"_qs, 48);
  QString txt = u"qofojg wofob"_qs;

  auto provider = std::make_shared<QCharRenderableFactory>(fnt);
  auto skin = std::make_shared<ClassicSkin>(provider);


  QConicalGradient g1(0.5, 0.5, 45.0);
  g1.setStops({
    {0.00, {170,   0,   0}},  // #aa0000
    {0.20, {  0,  85, 255}},  // #0055ff
    {0.45, {  0, 170,   0}},  // #00aa00
    {0.65, {255, 255,   0}},  // #ffff00
    {1.00, {170,   0,   0}},  // #aa0000
  });
  auto effect1 = std::make_shared<TexturingEffect>();
  effect1->setBrush(g1);

  QLinearGradient g2(0., 0., 0., 1.);
  g2.setColorAt(0.0, Qt::blue);
  g2.setColorAt(0.3, Qt::transparent);
  g2.setColorAt(1.0, Qt::transparent);
  g2.setCoordinateMode(QGradient::ObjectMode);
  auto effect2 = std::make_shared<TexturingEffect>();
  effect2->setBrush(g2);

  skin->addItemEffect(effect1);
  skin->addItemEffect(effect2);

  _clock = std::make_unique<ClockWidget>(txt, skin);

  connect(_timer, &QTimer::timeout, this, [this]() { _clock->setSeparatorVisible(!_clock->isSeparatorVisible()); });
  connect(_timer, &QTimer::timeout, this, qOverload<>(&QWidget::update));

  using namespace std::chrono_literals;
  _timer->start(500ms);
}

Widget::~Widget()
{
  _timer->stop();
}

void Widget::paintEvent(QPaintEvent* e)
{
  QPainter p(this);

  p.setPen(Qt::red);
  p.drawRect(rect());

  p.setPen(Qt::black);
  p.drawLine(0, height() / 2, width(), height() / 2);
  p.drawLine(width() / 2, 0, width() / 2, height());

  p.translate(rect().center());

  QFont fnt(u"Bad Script"_qs, 72);
//  QFont fnt(u"Segoe Script"_qs, 48);
  QString txt = u"qofojg wofob"_qs;

  QFontMetricsF fmf(fnt);
  QRectF tbr = fmf.boundingRect(txt);
  p.translate(-tbr.width() / 2., 0.);
  qDebug() << "text br h:" << tbr.height()
           << "font h:" << fmf.height()
           << "line spacing:" << fmf.lineSpacing();
  p.setPen(Qt::cyan);
  p.drawRect(tbr);

  // ********* reference code *********
  p.save();
  p.setFont(fnt);

  for (const auto& ch : txt) {
    QRectF br = fmf.boundingRect(ch);
    p.setPen(Qt::blue);
    p.drawRect(br);
    p.setPen(palette().color(QPalette::Text));
    p.drawText(QPointF(0., 0.), QString(ch));
    p.setPen(Qt::green);
    p.drawLine(QPointF(br.left(), 0.), QPointF(br.right(), 0.));
    p.drawLine(QPointF(0., br.top()), QPointF(0., br.bottom()));
    p.translate(fmf.horizontalAdvance(ch), 0.);
  }
  p.restore();

  // ********* test code *********
  p.setPen(Qt::yellow);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  _clock->render(&p);

  e->accept();
}
