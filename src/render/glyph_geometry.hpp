#pragma once

#include <optional>

#include <QRectF>

struct GlyphGeometryRaw {
  std::optional<qreal> x;
  std::optional<qreal> y;
  std::optional<qreal> w;
  std::optional<qreal> h;
  std::optional<qreal> ax;
  std::optional<qreal> ay;
};

class GlyphGeometry final
{
public:
  constexpr GlyphGeometry(const GlyphGeometryRaw& g, const QSizeF& sz) noexcept
    : m_rect(g.x.value_or(0.0), g.y.value_or(0.0),
             g.w.value_or(sz.width()), g.h.value_or(sz.height()))
    , m_ax(g.ax.value_or(m_rect.width()))
    , m_ay(g.ay.value_or(m_rect.height()))
  {}

  constexpr GlyphGeometry() noexcept : GlyphGeometry({}, {}) {}

  constexpr const QRectF& rect() const noexcept { return m_rect; }
  constexpr qreal advanceX() const noexcept { return m_ax; }
  constexpr qreal advanceY() const noexcept { return m_ay; }

private:
  QRectF m_rect;
  qreal m_ax;
  qreal m_ay;
};
