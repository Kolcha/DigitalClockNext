#pragma once

#include <QtGui/QIconEngine>

class ClockIconEngine : public QIconEngine
{
public:
  ClockIconEngine();
  ~ClockIconEngine();

  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  QIconEngine* clone() const override;

  void setColor(const QColor& color);
  void setFractionalHours(bool enable);

  const QColor& color() const noexcept;
  bool fractionalHours() const noexcept;

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};
