#include "char_renderable.hpp"

#include <QFontMetricsF>
#include <QString>

#include "core/hasher.hpp"
#include "render/state_guard_qpainter.hpp"

QCharRenderable::QCharRenderable(QChar ch, const QFont& font)
  : _ch(ch)
  , _font(font)
  , _hash(hasher(ch, font))
{
  QFontMetricsF fmf(font);
  _br = fmf.boundingRect(ch);
  _ax = fmf.horizontalAdvance(ch);
  _ay = fmf.lineSpacing();
}

void QCharRenderable::render(QPainter* p)
{
  StateGuard _(p);
  p->setFont(_font);
  p->drawText(QPointF(0., 0.), QString(_ch));
}
