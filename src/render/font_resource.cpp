#include "font_resource.hpp"

#include <QFontMetricsF>
#include <QPainter>

FontResource::FontResource(const QFont& font, char32_t ch)
    : _ch(ch)
    , _font(font)
    , _hash(qHashMulti(0, font, ch))
{
  QFontMetricsF fmf(font);
  if (ch <= 0xFFFF) {
    _br = fmf.boundingRect(QChar(ch));
    _ax = fmf.horizontalAdvance(QChar(ch));
  } else {
    _br = fmf.tightBoundingRect(QString::fromUcs4(&ch, 1));
    _ax = (_font.italic() ? 0.8 : 1.0) * _br.width();
  }
  _ay = fmf.lineSpacing();
}

void FontResource::draw(QPainter* p)
{
  p->save();
  p->setFont(_font);
  p->drawText(0, 0, QString::fromUcs4(&_ch, 1));
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
