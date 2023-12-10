/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QWidget>

#include <memory>

#include <QDateTime>
#include <QTimeZone>

class ClockSkin;

class ClockWidgetWrap : public QWidget
{
  Q_OBJECT

  using SkinPtr = std::shared_ptr<ClockSkin>;

public:
  explicit ClockWidgetWrap(QWidget* parent = nullptr);
  ~ClockWidgetWrap();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  void setSkin(std::shared_ptr<ClockSkin> skin);
  std::shared_ptr<ClockSkin> skin() const;

public slots:
  void setDateTime(const QDateTime& dt);
  void setTimeZone(const QTimeZone& tz);

  void animateSeparator();

  void scale(qreal kx, qreal ky);

  void skinConfigured();

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  struct impl;
  std::unique_ptr<impl> _impl;
};
