#include "image_resource.hpp"

#include <QPainter>

void RasterImageResource::draw(QPainter* p)
{
  p->drawPixmap(rect().toRect(), m_icon.pixmap(m_size));
}

void SvgImageResource::draw(QPainter* p)
{
  m_renderer->render(p, rect());
}
