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

#define DECLARE_STATE_PROPERTY(type, name)  \
  virtual void set##name(const type& value) = 0;  \
  virtual type get##name() const = 0;

class QDateTime;
class QTimeZone;

class Skin;

class ClockWindow : public QWidget
{
  Q_OBJECT

public:
  class State
  {
  public:
    virtual ~State();
    DECLARE_STATE_PROPERTY(QPoint, Pos)
    DECLARE_STATE_PROPERTY(Qt::Alignment, Alignment)
  };

private:
  using StatePtr = std::unique_ptr<State>;
  using SkinPtr = std::shared_ptr<Skin>;

public:
  // TODO: remove state from constructor - app should assign required properties instead
  explicit ClockWindow(StatePtr state, QWidget* parent = nullptr);
  ~ClockWindow();

  bool isSeparatorFlashes() const;

  void setSkin(std::shared_ptr<Skin> skin);
  std::shared_ptr<Skin> skin() const;

signals:
  void settingsDialogRequested();
  void aboutDialogRequested();
  void appExitRequested();

public slots:
  void setDateTime(const QDateTime& utc);
  void setTimeZone(const QTimeZone& tz);

  void setSeparatorFlashes(bool flashes);

  void animateSeparator();

  void scale(int sx, int sy); // in percents

  void setAlignment(Qt::Alignment alignment);

  void setSnapToEdge(bool enable);
  void setSnapThreshold(int threshold);

  // TODO: make Skin QObject?
  void skinConfigured();

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void showEvent(QShowEvent* event) override;

private:
  struct impl;
  std::unique_ptr<impl> _impl;
};
