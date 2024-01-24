/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023-2024  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "linear_layout.hpp"

#include <algorithm>
#include <iterator>

std::pair<qreal, qreal> LinearLayout::doBuildLayout()
{
  Q_ASSERT(!_items.empty());
  resetPosInOppositeDirection();
  auto [omin, omax] = resizeItems();
  // position items
  auto prev = _items.begin();
  (*prev)->setPos({0, 0});  // the first item is always at (0,0)
  applyAlignment(0, omin, omax);

  for (auto iter = std::next(prev); iter != _items.end(); ++iter) {
    applyAlignment(std::distance(_items.begin(), iter), omin, omax);
    const auto& item = *iter;
    // positioning
    qreal dpos = ((*prev)->pos().*_orientation->cpos)();
    if (_ignore_advance) {
      dpos += ((*prev)->rect().*_orientation->cmax)();
      dpos -= ((*iter)->rect().*_orientation->cmin)();
    } else {
      dpos += _orientation->cadvance(**iter, **prev);
    }
    auto pos = item->pos();
    (pos.*_orientation->cpos)() = dpos + _spacing;
    item->setPos(std::move(pos));
    // current -> previous
    prev = iter;
  }

  return _orientation->advances(_items);
}

std::pair<qreal, qreal> LinearLayout::resizeItems()
{
  // find min/max y for non-resizeable items
  auto f_iter = std::find_if_not(
                  _items.begin(), _items.end(),
                  [](const auto& i) { return i->resizeEnabled(); }
  );
  const auto& f_item = f_iter == _items.end() ? _items.front() : *f_iter;

  qreal omin = (f_item->rect().*_orientation->omin)();
  qreal omax = (f_item->rect().*_orientation->omax)();

  for (const auto& item : _items) {
    if (item->resizeEnabled()) continue;
    omin = std::min(omin, (item->rect().*_orientation->omin)());
    omax = std::max(omax, (item->rect().*_orientation->omax)());
  }

  // resize resizeable items
  for (const auto& item : _items) {
    if (!item->resizeEnabled()) continue;
    item->resize(omax - omin, _orientation->opposite);
    auto pos = item->pos();
    (pos.*_orientation->opos)() = omin - (item->rect().*_orientation->omin)();
    item->setPos(std::move(pos));
  }

  return {omin, omax};
}

void LinearLayout::applyAlignment(LayoutItem& item, Qt::Alignment a, const QRectF& g)
{
  // alignment can be applied only to non-resizeable items
  if (item.resizeEnabled()) return;

  const auto& r = item.rect();
  auto halign = a & Qt::AlignHorizontal_Mask;
  auto valign = a & Qt::AlignVertical_Mask;
  qreal dx = 0;
  qreal dy = 0;
  if (halign == Qt::AlignLeft) dx = g.left() - r.left();
  if (halign == Qt::AlignHCenter) dx = g.center().x() - r.center().x();
  if (halign == Qt::AlignRight) dx = g.right() - r.right();
  if (valign == Qt::AlignTop) dy = g.top() - r.top();
  if (valign == Qt::AlignVCenter) dy = g.center().y() - r.center().y();
  if (valign == Qt::AlignBottom) dy = g.bottom() - r.bottom();

  item.setPos(item.pos() + QPointF(dx, dy));
}

void LinearLayout::resetPosInOppositeDirection()
{
  for (const auto& item : _items) {
    auto pos = item->pos();
    (pos.*_orientation->opos)() = 0;
    item->setPos(std::move(pos));
  }
}

namespace h {

static QRectF geometry(const QRectF& r, qreal omin, qreal omax)
{
  return QRectF(r.x(), omin, r.width(), omax - omin);
}

static std::pair<qreal, qreal> advances(const auto& items)
{
  qreal max_y_this_line = items.front()->pos().y();
  qreal min_y_prev_line = max_y_this_line - items.front()->ay();

  for (const auto& item : items) {
    max_y_this_line = std::max(max_y_this_line, item->pos().y());
    min_y_prev_line = std::min(min_y_prev_line, item->pos().y() - item->ay());
  }

  qreal ax = items.back()->pos().x() - items.front()->pos().x() + items.back()->ax();
  return {ax, max_y_this_line - min_y_prev_line};
}

} // namespace h

namespace v {

static QRectF geometry(const QRectF& r, qreal omin, qreal omax)
{
  return QRectF(omin, r.y(), omax - omin, r.height());
}

static std::pair<qreal, qreal> advances(const auto& items)
{
  qreal min_x_this_line = items.front()->pos().x();
  qreal max_x_next_line = min_x_this_line + items.front()->ax();

  for (const auto& item : items) {
    min_x_this_line = std::min(min_x_this_line, item->pos().x());
    max_x_next_line = std::max(max_x_next_line, item->pos().x() + item->ax());
  }

  qreal ay = items.back()->pos().y() - items.front()->pos().y() + items.front()->ay();
  return {max_x_next_line - min_x_this_line, ay};
}

} // namespace v

const LinearLayout::Orientation LinearLayout::horizontal {
  Qt::Horizontal,
  Qt::Vertical,
  &QRectF::left,
  &QRectF::right,
  &QRectF::top,
  &QRectF::bottom,
  &QPointF::rx,
  &QPointF::ry,
  [](const LayoutItem&, const LayoutItem& prev){ return prev.ax(); },
  &h::geometry,
  &h::advances,
};

const LinearLayout::Orientation LinearLayout::vertical {
  Qt::Vertical,
  Qt::Horizontal,
  &QRectF::top,
  &QRectF::bottom,
  &QRectF::left,
  &QRectF::right,
  &QPointF::ry,
  &QPointF::rx,
  [](const LayoutItem& curr, const LayoutItem&){ return curr.ay(); },
  &v::geometry,
  &v::advances,
};
