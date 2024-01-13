#pragma once

#include <memory>

#include <QHash>
#include <QRect>

class QPainter;

// "skin resource"
class Resource {
public:
  virtual ~Resource() = default;

  virtual QRectF rect() const = 0;
  virtual qreal advanceX() const = 0;
  virtual qreal advanceY() const = 0;

  virtual void draw(QPainter* p) = 0;

  virtual size_t cacheKey() const = 0;
};


class ResourceFactory {
public:
  virtual ~ResourceFactory() = default;

  std::shared_ptr<Resource> item(char32_t ch) const
  {
    auto& resource = _cache[ch];
    if (!resource) resource = create(ch);
    return resource;
  }

  virtual qreal ascent() const = 0;
  virtual qreal descent() const = 0;

  inline qreal height() const { return ascent() + descent(); }

protected:
  virtual std::shared_ptr<Resource> create(char32_t ch) const = 0;

private:
  mutable QHash<char32_t, std::shared_ptr<Resource>> _cache;
};
