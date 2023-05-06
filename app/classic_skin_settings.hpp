#pragma once

#include <QWidget>

namespace Ui {
class ClassicSkinSettings;
}

class ClassicSkinSettings : public QWidget
{
  Q_OBJECT

public:
  explicit ClassicSkinSettings(QWidget* parent = nullptr);
  ~ClassicSkinSettings();

private:
  Ui::ClassicSkinSettings* ui;
};
