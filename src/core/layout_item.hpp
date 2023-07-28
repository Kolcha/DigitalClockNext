#pragma once

#include <memory>

#include <QRectF>
#include <QTransform>

class LayoutItem : public std::enable_shared_from_this<LayoutItem> {
public:
  virtual ~LayoutItem() = default;

  // means "original bounding rect", i.e. as was returned from the source
  QRectF rect() const { return _rect; }

  virtual qreal advanceX() const { return _rect.width(); }
  virtual qreal advanceY() const { return _rect.height(); }

  QTransform transform() const { return _transform; }

  // TODO: cache geometry? is it worth? what about rect change?
  QRectF geometry() const { return _transform.mapRect(_rect); }

protected:
  void setRect(QRectF r)
  {
    _rect = std::move(r);

    if (auto parent = _parent.lock())
      parent->updateGeometry();
  }

private:
  friend class LayoutItemModifier;
  void setParent(const std::shared_ptr<LayoutItem>& p) { _parent = p; }
  void setTransform(QTransform t) { _transform = std::move(t); }

private:
  virtual void updateGeometry() {}

private:
  QRectF _rect;
  QTransform _transform;
  std::weak_ptr<LayoutItem> _parent;
};
