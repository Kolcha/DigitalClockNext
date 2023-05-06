#include "classic_skin_settings.hpp"
#include "ui_classic_skin_settings.h"

ClassicSkinSettings::ClassicSkinSettings(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::ClassicSkinSettings)
{
  ui->setupUi(this);
}

ClassicSkinSettings::~ClassicSkinSettings()
{
  delete ui;
}
