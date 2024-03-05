/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <memory>
#include <vector>

#include <QTransform>

#include "effect.hpp"
#include "resource.hpp"

// implements geometry changes propagation,
// geometry caching, and other common tasks
class LayoutItem : public std::enable_shared_from_this<LayoutItem> {
public:
  explicit LayoutItem(std::shared_ptr<Resource> res);

  LayoutItem(const LayoutItem&) = delete;
  LayoutItem(LayoutItem&&) = default;

  LayoutItem& operator=(const LayoutItem&) = delete;
  LayoutItem& operator=(LayoutItem&&) = default;

  virtual ~LayoutItem() = default;

  QRectF rect() const { return _rect; }
  qreal ax() const { return _ax; }
  qreal ay() const { return _ay; }

  std::shared_ptr<Resource> resource() const { return _res; }

  QPointF pos() const { return _pos; }
  void setPos(QPointF p) { _pos = std::move(p); }

  QTransform transform() const
  {
    return QTransform(_transform).scale(_ks, _ks);
  }

  void setTransform(QTransform t)
  {
    _transform = std::move(t);
    updateCachedGeometry();
  }

  // is it really requred?
  std::shared_ptr<LayoutItem> parent() const { return _parent.lock(); }
  void setParent(std::weak_ptr<LayoutItem> p) { _parent = std::move(p); }

  void updateGeometry();

  // layout stuff
  bool resizeEnabled() const { return _resize_enabled; }
  void setResizeEnabled(bool enabled);
  inline void enableResize() { setResizeEnabled(true); }
  inline void disableResize() { setResizeEnabled(false); }

  // resize item in given direction
  // aspect ratio is preserved
  void resize(qreal l, Qt::Orientation o);

  // decorate resource with effect
  void decorate(std::shared_ptr<Effect> effect)
  {
    _res = effect->decorate(std::move(_res));
    updateCachedGeometry();
  }

protected:
  virtual void doUpdateGeometry() {}

private:
  void updateCachedGeometry();

private:
  QPointF _pos = QPointF(0, 0);
  QTransform _transform;
  std::weak_ptr<LayoutItem> _parent;
  // resource
  std::shared_ptr<Resource> _res;
  // transformed resource geometry
  QRectF _rect;
  qreal _ax = 0;
  qreal _ay = 0;
  // is resizing enabled?
  bool _resize_enabled = false;
  // scaling coefficient to achive "resize effect"
  qreal _ks = 1.0;
};


// base class for all layout implementations
class Layout : public LayoutItem {
public:
  void addItem(std::shared_ptr<LayoutItem> item)
  {
    Q_ASSERT(item);
    item->setParent(weak_from_this());
    doAddItem(item);
    _res->addItem(std::move(item));
  }

  const auto& items() const noexcept { return _res->items(); }

protected:
  Layout();

  // it should be "final", but kept "override" for testing purposes
  void doUpdateGeometry() override
  {
    auto [ax, ay] = doBuildLayout();
    _res->updateGeometry(ax, ay);
  }

  virtual void doAddItem(std::shared_ptr<LayoutItem> item) = 0;
  // returns (ax,ay)
  virtual std::pair<qreal, qreal> doBuildLayout() = 0;

private:
  // integral part of layout implementation
  // should not be a part of public API
  class LayoutResource : public Resource {
  public:
    QRectF rect() const override { return _rect; }
    qreal advanceX() const override { return _ax; }
    qreal advanceY() const override { return _ay; }

    void draw(QPainter* p) override;

    size_t cacheKey() const override;

    void addItem(std::shared_ptr<LayoutItem> item)
    {
      _items.push_back(std::move(item));
    }

    using Items = std::vector<std::shared_ptr<LayoutItem>>;
    const Items& items() const noexcept { return _items; }

    // there is no way to guess ax/ay,
    // only layout may know them,
    // so they must be passed explicitly
    void updateGeometry(qreal ax, qreal ay);

  private:
    std::vector<std::shared_ptr<LayoutItem>> _items;

    QRectF _rect;
    qreal _ax = 0;
    qreal _ay = 0;
  };

  Layout(std::shared_ptr<LayoutResource> res);

private:
  std::shared_ptr<LayoutResource> _res;
};


// fixed-size item which can hold another item
class PlaceholderItem : public LayoutItem {
public:
  PlaceholderItem(QRectF r, qreal ax, qreal ay)
    : PlaceholderItem(std::make_shared<PlaceholderResource>(r, ax, ay))
  {
    Q_ASSERT(_res);
  }

  void setContent(std::shared_ptr<LayoutItem> item)
  {
    if (item)
      item->setParent(weak_from_this());
    _res->setContent(std::move(item));
  }

  void setContentAlignment(Qt::Alignment a) noexcept { _alignment = a; }

protected:
  void doUpdateGeometry() override;

private:
  // integral part of implementation
  // should not be a part of public API
  class PlaceholderResource : public Resource {
  public:
    PlaceholderResource(QRectF r, qreal ax, qreal ay) noexcept
      : _r(std::move(r))
      , _ax(ax)
      , _ay(ay)
    {}

    QRectF rect() const noexcept override { return _r; }
    qreal advanceX() const noexcept override { return _ax; }
    qreal advanceY() const noexcept override { return _ay; }

    void draw(QPainter* p) override;

    size_t cacheKey() const noexcept override { return -1; }

    void setContent(std::shared_ptr<LayoutItem> item) noexcept
    {
      _item = std::move(item);
    }

    auto content() const noexcept { return _item; }

  private:
    QRectF _r;
    qreal _ax;
    qreal _ay;
    std::shared_ptr<LayoutItem> _item;
  };

private:
  PlaceholderItem(std::shared_ptr<PlaceholderResource> res)
    : LayoutItem(res)
    , _res(std::move(res))
  {}

private:
  std::shared_ptr<PlaceholderResource> _res;
  Qt::Alignment _alignment = Qt::AlignCenter;
};
