#pragma once

#include <QWidget>

#include <memory>

class ClockSkin;
class ClockWidget;

class ClockWidgetWrap : public QWidget
{
  Q_OBJECT

  using SkinPtr = std::shared_ptr<ClockSkin>;

public:
  explicit ClockWidgetWrap(const SkinPtr& skin, const QDateTime& dt, QWidget* parent = nullptr);
  ~ClockWidgetWrap();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  bool isSeparatorVisible() const;

  void setSkin(std::shared_ptr<ClockSkin> skin);

public slots:
  void setDateTime(const QDateTime& dt);
  void setSeparatorVisible(bool visible);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  std::unique_ptr<ClockWidget> _impl;
};
