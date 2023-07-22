#include "window_positioning.hpp"

QPoint pick_ref_point(const QRect& r, Qt::Alignment a) noexcept
{
  int x = r.x();
  int y = r.y();

  switch (a & Qt::AlignHorizontal_Mask) {
    case Qt::AlignLeft:
      x = r.x();
      break;
    case Qt::AlignHCenter:
      x = r.x() + r.width() / 2;
      break;
    case Qt::AlignRight:
      x = r.x() + r.width();
      break;
  }

  switch (a & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
      y = r.y();
      break;
    case Qt::AlignVCenter:
      y = r.y() + r.height() / 2;
      break;
    case Qt::AlignBottom:
      y = r.y() + r.height();
      break;
  }

  return QPoint(x, y);
}

QRect aligned_rect(const QRect& r, const QPoint& p, Qt::Alignment a) noexcept
{
  int dx = 0;
  int dy = 0;

  switch (a & Qt::AlignHorizontal_Mask) {
    case Qt::AlignLeft:
      dx = p.x() - r.x();
      break;
    case Qt::AlignHCenter:
      dx = p.x() - r.width() / 2 - r.x();
      break;
    case Qt::AlignRight:
      dx = p.x() - r.x() - r.width();
      break;
  }

  switch (a & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
      dy = p.y() - r.y();
      break;
    case Qt::AlignVCenter:
      dy = p.y() - r.height() / 2 - r.y();
      break;
    case Qt::AlignBottom:
      dy = p.y() - r.y() - r.height();
      break;
  }

  return r.translated(dx, dy);
}
