#pragma once

#include <memory>

#include <QRectF>
#include <QTransform>

class QPainter;

// pure virtual, as used by several different entities
class Glyph : public std::enable_shared_from_this<Glyph> {
public:
  virtual ~Glyph() = default;

  // original rect (self rect)
  virtual QRectF rect() const = 0;
  // bounding rect (transform applied)
  virtual QRectF boundingRect() const = 0;

  // distance to next item (transform applied)
  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  // item's transform
  virtual QTransform transform() const = 0;

  // position in parent item
  virtual QPointF pos() const = 0;
  // geometry in parent item (available space)
  virtual QRectF geometry() const = 0;

  // draw item
  virtual void draw(QPainter* p) = 0;

  // drawing attributes
  virtual Qt::Alignment alignment() const = 0;

  virtual bool isVisible() const = 0;
  virtual void setVisible(bool visible) = 0;

  // should not propagate geometry change,
  // but rather update own geometry
  // item's transform change
  virtual void setTransform(QTransform t) = 0;

  // geometry change
  virtual void setPos(QPointF p) = 0;
  virtual void setGeometry(QRectF g) = 0;
  // recalculate own geometry and propagate change
  virtual void updateGeometry() = 0;

  // drawing attributes change
  virtual void setAlignment(Qt::Alignment a) = 0;

  // hierarchy support
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

  QRectF rect() const final { return _glyph->rect(); }
  QRectF boundingRect() const override { return _glyph->boundingRect(); }

  qreal advanceX() const override { return _glyph->advanceX(); }
  qreal advanceY() const override { return _glyph->advanceY(); }

  QTransform transform() const final { return _glyph->transform(); }

  QPointF pos() const override { return _glyph->pos(); }
  QRectF geometry() const override { return _glyph->geometry(); }

  void draw(QPainter* p) override { _glyph->draw(p); }

  Qt::Alignment alignment() const override { return _glyph->alignment(); }

  bool isVisible() const override { return _glyph->isVisible(); }
  void setVisible(bool visible) override { _glyph->setVisible(visible); }

  void setTransform(QTransform t) override
  {
    _glyph->setTransform(std::move(t));
  }

  void setPos(QPointF p) override { _glyph->setPos(std::move(p)); }

  void setGeometry(QRectF g) override
  {
    _glyph->setGeometry(std::move(g));
  }

  void updateGeometry() override { _glyph->updateGeometry(); }

  void setAlignment(Qt::Alignment a) override { _glyph->setAlignment(a); }

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
