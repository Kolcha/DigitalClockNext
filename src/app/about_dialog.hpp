#pragma once

#include <QDialog>

class QLabel;

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog();

protected:
  void showEvent(QShowEvent* event) override;

private:
  Ui::AboutDialog* ui;
  QLabel* _u_btn;
};
