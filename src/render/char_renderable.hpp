#pragma once

#include "core/skin_resource.hpp"

#include <QFont>
#include <QFontMetricsF>
#include <QPainter>
#include <QString>

#include "render/state_guard_qpainter.hpp"

class QCharRenderable final : public SkinResource {
  friend size_t qHashImpl(const QCharRenderable& r, size_t seed);

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

  void render(QPainter* p) override
  {
    StateGuard _(p);
    p->setFont(_font);
    p->drawText(QPointF(0., 0.), QString(_ch));
  }

private:
  QChar _ch;
  QRectF _br;
  qreal _ax;
  qreal _ay;
  const QFont& _font;
};
