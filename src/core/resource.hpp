#pragma once

#include <memory>

#include <QChar>
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

  std::shared_ptr<Resource> item(QChar ch) const
  {
    auto& resource = _cache[ch];
    if (!resource) resource = create(ch);
    return resource;
  }

protected:
  virtual std::shared_ptr<Resource> create(QChar ch) const = 0;

private:
  mutable QHash<QChar, std::shared_ptr<Resource>> _cache;
};
