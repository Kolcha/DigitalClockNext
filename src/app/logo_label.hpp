#pragma once

#include <QLabel>

#include <memory>

class LogoLabel final : public QLabel
{
  Q_OBJECT

public:
  explicit LogoLabel(QWidget* parent = nullptr);
  ~LogoLabel();

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
