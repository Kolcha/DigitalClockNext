#pragma once

#include "render/skin_resource.hpp"

#include <memory>

#include <QIcon>
#include <QPainter>
#include <QSvgRenderer>

class LegacyImageRenderable : public SkinResource {
public:
  explicit LegacyImageRenderable(const QString& filename)
    : m_filename(filename)
    , m_hash(qHash(filename))
  {}

  std::size_t hash() const noexcept override { return m_hash; }

  qreal advanceX() const override { return rect().width(); }
  qreal advanceY() const override { return rect().height(); }

private:
  QString m_filename;
  std::size_t m_hash;   // do not calculate hash every time
};


class RasterImageRenderable : public LegacyImageRenderable
{
public:
  explicit RasterImageRenderable(const QString& filename)
    : LegacyImageRenderable(filename)
    , m_icon(filename)
    , m_size(m_icon.availableSizes().constFirst())
  {}

  QRectF rect() const override
  {
    return QRectF(QPointF(0, 0), m_size);
  }

  void render(QPainter* p) override
  {
    p->drawPixmap(QPointF(0, 0), m_icon.pixmap(m_size));
  }

private:
  QIcon m_icon;   // QIcon perfectly handles HighDPI
  QSize m_size;   // 100% image size
};


class SvgImageRenderable : public LegacyImageRenderable
{
public:
  explicit SvgImageRenderable(const QString& filename)
    : LegacyImageRenderable(filename)
    , m_renderer(std::make_unique<QSvgRenderer>(filename))
  {}

  QRectF rect() const override
  {
    return QRectF(QPointF(0, 0), m_renderer->defaultSize());
  }

  void render(QPainter* p) override
  {
    m_renderer->render(p, rect());
  }

private:
  std::unique_ptr<QSvgRenderer> m_renderer;
};
