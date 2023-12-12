#pragma once

#include "glyph.hpp"

#include "geometry.hpp"
#include "layout_algorithm.hpp"
#include "resource.hpp"

// common logic for "resource-based" glyphs
// implements geometry changes propagation,
// geometry caching, and other common tasks
class GlyphBase : public Glyph {
public:
  GlyphBase() noexcept = default;
  GlyphBase& operator=(GlyphBase &&other) noexcept = default;
  GlyphBase& operator=(const GlyphBase &) noexcept = delete;
  GlyphBase(GlyphBase&& other) noexcept = default;
  GlyphBase(const GlyphBase& other) noexcept = delete;

  QRectF geometry() const noexcept final { return _curr_geom.rect(); }
  qreal advanceX() const noexcept final { return _curr_geom.advanceX(); }
  qreal advanceY() const noexcept final { return _curr_geom.advanceY(); }

  QRectF rect() const noexcept final { return _init_geom.rect(); }
  QTransform transform() const noexcept final { return _transform; }

  void draw(QPainter* p) final;

  bool isVisible() const noexcept final { return _visible; }
  void setVisible(bool visible) noexcept final { _visible = visible; }

  void setGeometry(QRectF r, qreal ax, qreal ay) final
  {
    _init_geom.setRect(std::move(r));
    _init_geom.setAdvance(ax, ay);
    updateCachedGeometry();
  }

  void setTransform(QTransform t) final
  {
    _transform = std::move(t);
    updateCachedGeometry();
  }

  void updateGeometry() final;

  void resetGeometry() noexcept override
  {
    _curr_geom = _init_geom;
    _transform = QTransform();
  }

  std::shared_ptr<Glyph> parent() const noexcept final { return _parent.lock(); }
  void setParent(std::weak_ptr<Glyph> p) noexcept final { _parent = std::move(p); }

protected:
  virtual void doUpdateGeometry() { updateCachedGeometry(); }

  virtual void doDraw(QPainter* p) = 0;

private:
  void updateCachedGeometry();

private:
  QTransform _transform;
  Geometry _init_geom;
  Geometry _curr_geom;
  bool _visible = true;
  std::weak_ptr<Glyph> _parent;
};


// implements geometry using "skin resource"
class SimpleGlyph : public GlyphBase {
public:
  explicit SimpleGlyph(std::shared_ptr<Resource> r)
      : _res(std::move(r))
  {
    Q_ASSERT(_res);
    setGeometry(_res->rect(), _res->advanceX(), _res->advanceY());
  }

  size_t cacheKey() const override { return _res->cacheKey(); }

protected:
  void doDraw(QPainter* p) override { _res->draw(p); }

private:
  std::shared_ptr<Resource> _res;
};


// "layout"
class CompositeGlyph : public GlyphBase {
public:
  explicit CompositeGlyph(std::shared_ptr<LayoutAlgorithm> a = nullptr) noexcept
      : GlyphBase()
      , _algorithm(std::move(a))
  {}

  void addGlyph(std::shared_ptr<Glyph> g)
  {
    g->setParent(weak_from_this());
    _items.push_back(std::move(g));
  }

  void setAlgorithm(std::shared_ptr<LayoutAlgorithm> a) noexcept
  {
    _algorithm = std::move(a);
  }

  const auto& algorithm() const noexcept { return _algorithm; }
  const auto& items() const noexcept { return _items; }

  size_t cacheKey() const override;

protected:
  void doUpdateGeometry() override;

  void doDraw(QPainter* p) override
  {
    for (const auto& i : _items) i->draw(p);
  }

private:
  std::shared_ptr<LayoutAlgorithm> _algorithm;
  std::vector<std::shared_ptr<Glyph>> _items;
};
