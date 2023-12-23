#include "glyph.hpp"

#include <QPainter>
#include <QPixmapCache>

void CachedGlyph::draw(QPainter* p)
{
  if (!isVisible() || rect().isEmpty())
    return;

  p->save();
  auto ext_tr = p->transform();
  auto br = p->transform().mapRect(geometry());
  auto sz = (br.size() * p->device()->devicePixelRatioF()).toSize();

  QString key = QString("%1_%2x%3")
                    .arg(cacheKey())
                    .arg(sz.width()).arg(sz.height());
  QPixmap pxm;

  if (!QPixmapCache::find(key, &pxm)) {
    pxm = QPixmap(sz);
    pxm.setDevicePixelRatio(p->device()->devicePixelRatioF());
    pxm.fill(Qt::transparent);
    {
      QPainter pp(&pxm);
      pp.setRenderHints(p->renderHints());
      pp.translate(-br.topLeft());
      pp.setTransform(ext_tr, true);
      GlyphDecorator::draw(&pp);
    }
    QPixmapCache::insert(key, pxm);
  }
  p->resetTransform();
  p->translate(br.topLeft());
  p->drawPixmap(0, 0, pxm);
  p->restore();
}
