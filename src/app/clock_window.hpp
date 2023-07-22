#pragma once

#include <QWidget>

#include <memory>

#define DECLARE_STATE_PROPERTY(type, name)  \
  virtual void set##name(const type& value) = 0;  \
  virtual type get##name() const = 0;

class QDateTime;
class QTimeZone;

class ClockSkin;

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
  using SkinPtr = std::shared_ptr<ClockSkin>;

public:
  // dt should be in desired time zone, no sense to pass 2 arguments for this
  ClockWindow(const SkinPtr& skin, const QDateTime& dt, StatePtr state, QWidget* parent = nullptr);
  ~ClockWindow();

  bool isSeparatorVisible() const;
  bool isSeparatorFlashes() const;

  void setSkin(std::shared_ptr<ClockSkin> skin);
  std::shared_ptr<ClockSkin> skin() const;

signals:
  void settingsDialogRequested();
  void aboutDialogRequested();
  void appExitRequested();

public slots:
  void setDateTime(const QDateTime& utc);
  void setTimeZone(const QTimeZone& tz);
  void setSeparatorVisible(bool visible);
  void setSeparatorFlashes(bool flashes);

  void flipSeparator();   // show/hide

  void scale(int sx, int sy); // in percents

  void setAlignment(Qt::Alignment alignment);

  void setSnapToEdge(bool enable);
  void setSnapThreshold(int threshold);

  // TODO: make ClockSkin QObject?
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
