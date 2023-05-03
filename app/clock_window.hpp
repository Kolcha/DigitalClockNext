#pragma once

#include <QWidget>

#include <memory>

#include <QDateTime>
#include <QTimeZone>

class ClockSkin;

class ClockWindow : public QWidget
{
  Q_OBJECT

  using SkinPtr = std::shared_ptr<ClockSkin>;

public:
  // dt should be in desired time zone, no sense to pass 2 arguments for this
  ClockWindow(const SkinPtr& skin, const QDateTime& dt, QWidget* parent = nullptr);
  ~ClockWindow();

  bool isSeparatorVisible() const;

  void setSkin(std::shared_ptr<ClockSkin> skin);

public slots:
  void setDateTime(const QDateTime& utc);
  void setTimeZone(const QTimeZone& tz);
  void setSeparatorVisible(bool visible);

private:
  struct impl;
  std::unique_ptr<impl> _impl;
};