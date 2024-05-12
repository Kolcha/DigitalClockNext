/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

  void changeOpacityOnMouseHover(bool enable);
  void setOpacityOnMouseHover(qreal opacity);
  // if window is transparent for input, no input events are sent
  // even to application, so mouse should be tracked using platform API
  // handler should be public to keep only one instance of that tracker
  void handleMouseMove(const QPoint& global_pos);

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
