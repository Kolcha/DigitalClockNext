#pragma once

#include <memory>

#include <QRectF>
#include <QTransform>

class QPainter;

// pure virtual, as used by several different entities
class Glyph : public std::enable_shared_from_this<Glyph> {
public:
  virtual ~Glyph() = default;
  // read-only property, calculated
  virtual QRectF geometry() const = 0;
  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;
  // initial data for calculation (writable)
  virtual QRectF rect() const = 0;
  virtual QTransform transform() const = 0;

  virtual void draw(QPainter* p) = 0;

  virtual bool isVisible() const = 0;
  virtual void setVisible(bool visible) = 0;

  // should not propagate geometry change,
  // but rather update own geometry
  virtual void setGeometry(QRectF r, qreal ax, qreal ay) = 0;
  virtual void setTransform(QTransform t) = 0;

  // recalculate own geometry and propagate change
  virtual void updateGeometry() = 0;
  // restore item's initial geometry, drop transform
  virtual void resetGeometry() = 0;

  virtual std::shared_ptr<Glyph> parent() const = 0;
  virtual void setParent(std::weak_ptr<Glyph> p) = 0;

  // should provide something unique
  virtual size_t cacheKey() const = 0;
};


class GlyphDecorator : public Glyph {
public:
  explicit GlyphDecorator(std::shared_ptr<Glyph> g) noexcept
      : _glyph(std::move(g))
  {
    Q_ASSERT(_glyph);
  }

  QRectF geometry() const override { return _glyph->geometry(); }
  qreal advanceX() const override { return _glyph->advanceX(); }
  qreal advanceY() const override { return _glyph->advanceY(); }

  QRectF rect() const final { return _glyph->rect(); }
  QTransform transform() const final { return _glyph->transform(); }

  void draw(QPainter* p) override { _glyph->draw(p); }

  bool isVisible() const override { return _glyph->isVisible(); }
  void setVisible(bool visible) override { _glyph->setVisible(visible); }

  void setGeometry(QRectF r, qreal ax, qreal ay) override
  {
    _glyph->setGeometry(std::move(r), ax, ay);
  }

  void setTransform(QTransform t) override
  {
    _glyph->setTransform(std::move(t));
  }

  void updateGeometry() override { _glyph->updateGeometry(); }
  void resetGeometry() override { _glyph->resetGeometry(); }

  std::shared_ptr<Glyph> parent() const final
  {
    return _glyph->parent();
  }

  void setParent(std::weak_ptr<Glyph> p) override
  {
    _glyph->setParent(std::move(p));
  }

  size_t cacheKey() const override { return _glyph->cacheKey(); }

private:
  std::shared_ptr<Glyph> _glyph;
};


class CachedGlyph : public GlyphDecorator {
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) override;
};
