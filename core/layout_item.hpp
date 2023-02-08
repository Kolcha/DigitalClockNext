#pragma once

#include <utility>

#include <QRectF>
#include <QTransform>

//class Layout;

class LayoutItem {
public:
  virtual ~LayoutItem() = default;

  LayoutItem() = default;

  LayoutItem(const LayoutItem&) = delete;
  LayoutItem(LayoutItem&&) = default;

  LayoutItem& operator=(const LayoutItem&) = delete;
  LayoutItem& operator=(LayoutItem&&) = default;

  const QRectF& geometry() const noexcept { return _geometry; }
  const QTransform& transform() const noexcept { return _transform; }

  void setGeometry(QRectF r)
  {
    if (r == _geometry)
      return;

    _geometry = std::move(r);

    if (_parent)
      _parent->updateGeometry();
  }

  void setTransform(QTransform m)
  {
    _transform = std::move(m);
    setGeometry(_transform.mapRect(_rect));
  }

  // means "original bounding rect", i.e. as was returned from the source
  // this value will be used by layout as base value for transormation
  const QRectF& rect() const noexcept { return _rect; }

  virtual qreal advanceX() const { return _rect.width(); }
  virtual qreal advanceY() const { return _rect.height(); }

protected:
  virtual void updateGeometry() {}

  void setRect(QRectF r)
  {
    if (r == _rect)
      return;

    _rect = std::move(r);
    setGeometry(_rect);
  }

private:
  friend class Layout;
  LayoutItem* _parent = nullptr;
  QRectF _rect;
  QRectF _geometry;
  QTransform _transform;
};
