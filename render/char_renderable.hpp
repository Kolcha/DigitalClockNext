#pragma once

#include "render/renderable_base.hpp"

#include <QFont>
#include <QFontMetricsF>
#include <QPainter>
#include <QString>

#include "render/state_guard.hpp"

class QCharRenderable final : public RenderableBase {
public:
  QCharRenderable(QChar ch, const QFont& font)
    : _ch(ch)
    , _font(font)
  {
    QFontMetricsF fmf(font);
    _br = fmf.boundingRect(ch);
    _ax = fmf.horizontalAdvance(ch);
    _ay = fmf.lineSpacing();
  }

  QRectF rect() const override { return _br; }

  qreal advanceX() const override { return _ax; }
  qreal advanceY() const override { return _ay; }

protected:
  void renderImpl(QPainter* p) const override
  {
    StateGuard _(p);
    p->setFont(_font);
    // TODO: consider QStaticText
    p->drawText(QPointF(0., 0.), QString(_ch));
  }

private:
  QChar _ch;
  QRectF _br;
  qreal _ax;
  qreal _ay;
  const QFont& _font;
};
