#include "font_resource.hpp"

#include <QFontMetricsF>
#include <QPainter>

FontResource::FontResource(const QFont& font, QChar ch)
    : _ch(ch)
    , _font(font)
    , _hash(qHashMulti(0, font, ch))
{
  QFontMetricsF fmf(font);
  _br = fmf.boundingRect(ch);
  _ax = fmf.horizontalAdvance(ch);
  _ay = fmf.lineSpacing();
}

void FontResource::draw(QPainter* p)
{
  p->save();
  p->setFont(_font);
  p->drawText(0, 0, QString(_ch));
  p->restore();
}

qreal FontResourceFactory::ascent() const
{
  return QFontMetricsF(_font).ascent();
}

qreal FontResourceFactory::descent() const
{
  return QFontMetricsF(_font).descent();
}
