#pragma once

#include "glyph.hpp"

#include <QBrush>

// draws thin border around inner object
class BorderEffect final : public GlyphDecorator {
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) override;

  QColor color() const noexcept { return _color; }
  qreal width() const noexcept { return _width; }

  void setColor(QColor c) noexcept { _color = std::move(c); }
  void setWidth(qreal w) noexcept { _width = w; }

private:
  QColor _color = Qt::red;
  qreal _width = 1.0;
};


// draws "base line"
class BaseLineEffect final : public GlyphDecorator {
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) override;

  QColor color() const noexcept { return _color; }
  qreal width() const noexcept { return _width; }

  void setColor(QColor c) noexcept { _color = std::move(c); }
  void setWidth(qreal w) noexcept { _width = w; }

private:
  QColor _color = Qt::green;
  qreal _width = 1.0;
};


// creates new drawing surface and draws inner item on it
// new surface size is equal to the size of the orinal one
class NewSurfaceEffect final : public GlyphDecorator {
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) override;
};


// applies texture to inner item, tiles by default
class TexturingEffect final : public GlyphDecorator {
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) override;

  QBrush brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

  void setBrush(QBrush b) noexcept { _brush = std::move(b); }
  void setStretch(bool s) noexcept { _stretch = s; }

private:
  QBrush _brush = QColor(128, 64, 240);
  bool _stretch = false;
};


// fills background and draws inner item on it
class BackgroundEffect final : public GlyphDecorator {
public:
  using GlyphDecorator::GlyphDecorator;

  void draw(QPainter* p) override;

  QBrush brush() const noexcept { return _brush; }
  bool stretch() const noexcept { return _stretch; }

  void setBrush(QBrush b) noexcept { _brush = std::move(b); }
  void setStretch(bool s) noexcept { _stretch = s; }

private:
  QBrush _brush = QColor(240, 224, 64);
  bool _stretch = false;
};
