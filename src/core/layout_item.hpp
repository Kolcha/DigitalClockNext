#pragma once

#include <utility>

#include <QRectF>

class LayoutItem {
public:
  virtual ~LayoutItem() = default;

  LayoutItem() = default;

  LayoutItem(const LayoutItem&) = delete;
  LayoutItem(LayoutItem&&) = default;

  LayoutItem& operator=(const LayoutItem&) = delete;
  LayoutItem& operator=(LayoutItem&&) = default;

  const QRectF& geometry() const noexcept { return _geometry; }

  // means "original bounding rect", i.e. as was returned from the source
  const QRectF& rect() const noexcept { return _rect; }

  virtual qreal advanceX() const { return _rect.width(); }
  virtual qreal advanceY() const { return _rect.height(); }

  const QPointF& pos() const noexcept { return _pos; }

  void setPos(QPointF p)
  {
    if (p == _pos)
      return;

    _pos = std::move(p);

    setGeometry(rect().translated(_pos));
  }

  inline void setPos(qreal x, qreal y) { setPos({x, y}); }

  void move(qreal dx, qreal dy)
  {
    _pos.setX(_pos.x() + dx);
    _pos.setY(_pos.y() + dy);
    setGeometry(geometry().translated(dx, dy));
  }

  inline void move(QPointF dp) { move(dp.x(), dp.y()); }

  // notify layout about item's geometry change
  virtual void updateGeometry()
  {
    if (_parent)
      _parent->updateGeometry();
  }

protected:
  void setRect(QRectF r)
  {
    if (r == _rect)
      return;

    _rect = std::move(r);
    setGeometry(_rect.translated(_pos));
  }

private:
  void setGeometry(QRectF r)
  {
    if (r == _geometry)
      return;

    _geometry = std::move(r);
  }

private:
  friend class Layout;
  LayoutItem* _parent = nullptr;
  QPointF _pos;
  QRectF _rect;
  QRectF _geometry;
};
