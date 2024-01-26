/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include <memory>

#include <QDateTime>
#include <QTimeZone>

class Skin;

class ClockWidget : public QWidget
{
  Q_OBJECT

  using SkinPtr = std::shared_ptr<Skin>;

public:
  explicit ClockWidget(QWidget* parent = nullptr);
  ~ClockWidget();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  void setSkin(std::shared_ptr<Skin> skin);
  std::shared_ptr<Skin> skin() const;

public slots:
  void setDateTime(const QDateTime& dt);
  void setTimeZone(const QTimeZone& tz);

  void animateSeparator();

  void scale(qreal kx, qreal ky);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  struct impl;
  std::unique_ptr<impl> _impl;
};
