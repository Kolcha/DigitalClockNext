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
  ClockWidgetWrap(const SkinPtr& skin, const QDateTime& dt, QWidget* parent = nullptr);
  ~ClockWidgetWrap();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  bool isSeparatorVisible() const;

  void setSkin(std::shared_ptr<ClockSkin> skin);
  std::shared_ptr<ClockSkin> skin() const;

public slots:
  void setDateTime(const QDateTime& dt);
  void setTimeZone(const QTimeZone& tz);
  void setSeparatorVisible(bool visible);

  void scale(qreal kx, qreal ky);

  void skinConfigured();

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  struct impl;
  std::unique_ptr<impl> _impl;
};
