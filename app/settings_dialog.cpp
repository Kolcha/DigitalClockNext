#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
{
  ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}