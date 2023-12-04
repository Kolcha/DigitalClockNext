#pragma once

#include <QtGui/QIconEngine>

class ClockIconEngine : public QIconEngine
{
public:
  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  QIconEngine* clone() const override;
};
