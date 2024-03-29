/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "classic_skin_settings.hpp"
#include "ui_classic_skin_settings.h"

#include <QFileDialog>
#include <QColorDialog>
#include <QSignalBlocker>

#include <gradient_dialog.h>

#include "app_config.hpp"
#include "classic_skin.hpp"

struct ClassicSkinSettings::Impl {
  WindowConfig* wcfg;
  ClassicSkinConfig* scfg;

  ClassicSkin* skin;

  Impl(ClassicSkin* skin, WindowConfig* wcfg)
    : wcfg(wcfg)
    , scfg(&wcfg->classicSkin())
    , skin(skin)
    , last_path(QDir::homePath())
  {
    skin->disableCaching();
  }

  ~Impl()
  {
    skin->enableCaching();
  }

  QString last_path;
};

ClassicSkinSettings::ClassicSkinSettings(ClassicSkin* skin, WindowConfig* wcfg, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::ClassicSkinSettings)
  , impl(std::make_unique<Impl>(skin, wcfg))
{
  ui->setupUi(this);

  ui->orientation_cbox->addItem(tr("Horizontal"), Qt::Horizontal);
  ui->orientation_cbox->addItem(tr("Vertical"), Qt::Vertical);

  auto oidx = ui->orientation_cbox->findData(impl->scfg->getOrientation());
  ui->orientation_cbox->setCurrentIndex(oidx);

  QSignalBlocker _(ui->spacing_edit);
  ui->spacing_edit->setValue(impl->scfg->getSpacing());

  ui->ignore_advance_x->setChecked(impl->scfg->getIgnoreAdvanceX());
  ui->ignore_advance_y->setChecked(impl->scfg->getIgnoreAdvanceY());

  auto tx = impl->scfg->getTexture();
  ui->texture_group->setChecked(tx.style() != Qt::NoBrush);
  ui->tx_solid_color_rbtn->setChecked(tx.style() == Qt::SolidPattern);
  ui->tx_gradient_rbtn->setChecked(tx.gradient() != nullptr);
  ui->tx_pattern_rbtn->setChecked(tx.style() == Qt::TexturePattern);
  ui->tx_pattern_stretch->setChecked(impl->scfg->getTextureStretch());
  ui->tx_per_element_cb->setChecked(impl->scfg->getTexturePerElement());

  auto bg = impl->scfg->getBackground();
  ui->background_group->setChecked(bg.style() != Qt::NoBrush);
  ui->bg_solid_color_rbtn->setChecked(bg.style() == Qt::SolidPattern);
  ui->bg_gradient_rbtn->setChecked(bg.gradient() != nullptr);
  ui->bg_pattern_rbtn->setChecked(bg.style() == Qt::TexturePattern);
  ui->bg_pattern_stretch->setChecked(impl->scfg->getBackgroundStretch());
  ui->bg_per_element_cb->setChecked(impl->scfg->getBackgroundPerElement());
}

ClassicSkinSettings::~ClassicSkinSettings()
{
  delete ui;
}

void ClassicSkinSettings::on_orientation_cbox_activated(int index)
{
  auto orientation = ui->orientation_cbox->itemData(index).value<Qt::Orientation>();
  impl->skin->setOrientation(orientation);
  impl->scfg->setOrientation(orientation);
}

void ClassicSkinSettings::on_spacing_edit_valueChanged(int arg1)
{
  impl->skin->setSpacing(arg1);
  impl->scfg->setSpacing(arg1);
}

void ClassicSkinSettings::on_ignore_advance_x_clicked(bool checked)
{
  impl->skin->setIgnoreAdvanceX(checked);
  impl->scfg->setIgnoreAdvanceX(checked);
}

void ClassicSkinSettings::on_ignore_advance_y_clicked(bool checked)
{
  impl->skin->setIgnoreAdvanceY(checked);
  impl->scfg->setIgnoreAdvanceY(checked);
}

void ClassicSkinSettings::on_texture_group_clicked(bool checked)
{
  QBrush brush(Qt::NoBrush);
  if (checked) {
    if (ui->tx_solid_color_rbtn->isChecked())
      brush = QBrush(impl->wcfg->state().getTextureColor());

    if (ui->tx_gradient_rbtn->isChecked())
      brush = QBrush(impl->wcfg->state().getTextureGradient());

    if (ui->tx_pattern_rbtn->isChecked())
      brush = QBrush(impl->wcfg->state().getTexturePattern());
  }
  impl->scfg->setTexture(brush);
  impl->skin->setTexture(std::move(brush));
}

void ClassicSkinSettings::on_tx_solid_color_rbtn_clicked()
{
  impl->scfg->setTexture(impl->wcfg->state().getTextureColor());
  impl->skin->setTexture(impl->wcfg->state().getTextureColor());
}

void ClassicSkinSettings::on_tx_select_color_btn_clicked()
{
  auto color = QColorDialog::getColor(impl->wcfg->state().getTextureColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  impl->scfg->setTexture(color);
  impl->wcfg->state().setTextureColor(color);
  impl->skin->setTexture(std::move(color));
}

void ClassicSkinSettings::on_tx_gradient_rbtn_clicked()
{
  impl->scfg->setTexture(impl->wcfg->state().getTextureGradient());
  impl->skin->setTexture(impl->wcfg->state().getTextureGradient());
}

void ClassicSkinSettings::on_tx_select_gradient_btn_clicked()
{
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              impl->wcfg->state().getTextureGradient(),
                                              this);
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  impl->scfg->setTexture(gradient);
  impl->wcfg->state().setTextureGradient(gradient);
  impl->skin->setTexture(std::move(gradient));
}

void ClassicSkinSettings::on_tx_pattern_rbtn_clicked()
{
  impl->scfg->setTexture(impl->wcfg->state().getTexturePattern());
  impl->skin->setTexture(impl->wcfg->state().getTexturePattern());
}

void ClassicSkinSettings::on_tx_select_pattern_btn_clicked()
{
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           impl->last_path,
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;
  impl->last_path = file;
  QPixmap pxm(file);
  impl->scfg->setTexture(pxm);
  impl->wcfg->state().setTexturePattern(pxm);
  impl->skin->setTexture(std::move(pxm));
}

void ClassicSkinSettings::on_tx_pattern_stretch_clicked(bool checked)
{
  impl->scfg->setTextureStretch(checked);
  impl->skin->setTextureStretch(checked);
}

void ClassicSkinSettings::on_tx_per_element_cb_clicked(bool checked)
{
  impl->scfg->setTexturePerElement(checked);
  impl->skin->setTexturePerElement(checked);
}

void ClassicSkinSettings::on_background_group_clicked(bool checked)
{
  QBrush brush(Qt::NoBrush);
  if (checked) {
    if (ui->bg_solid_color_rbtn->isChecked())
      brush = QBrush(impl->wcfg->state().getBackgroundColor());

    if (ui->bg_gradient_rbtn->isChecked())
      brush = QBrush(impl->wcfg->state().getBackgroundGradient());

    if (ui->bg_pattern_rbtn->isChecked())
      brush = QBrush(impl->wcfg->state().getBackgroundPattern());
  }
  impl->scfg->setBackground(brush);
  impl->skin->setBackground(std::move(brush));
}

void ClassicSkinSettings::on_bg_solid_color_rbtn_clicked()
{
  impl->scfg->setBackground(impl->wcfg->state().getBackgroundColor());
  impl->skin->setBackground(impl->wcfg->state().getBackgroundColor());
}

void ClassicSkinSettings::on_bg_select_color_btn_clicked()
{
  auto color = QColorDialog::getColor(impl->wcfg->state().getBackgroundColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  impl->scfg->setBackground(color);
  impl->wcfg->state().setBackgroundColor(color);
  impl->skin->setBackground(std::move(color));
}

void ClassicSkinSettings::on_bg_gradient_rbtn_clicked()
{
  impl->scfg->setBackground(impl->wcfg->state().getBackgroundGradient());
  impl->skin->setBackground(impl->wcfg->state().getBackgroundGradient());
}

void ClassicSkinSettings::on_bg_select_gradient_btn_clicked()
{
  bool ok = false;
  auto gradient = GradientDialog::getGradient(&ok,
                                              impl->wcfg->state().getBackgroundGradient(),
                                              this);
  if (!ok) return;
  gradient.setCoordinateMode(QGradient::ObjectMode);
  impl->scfg->setBackground(gradient);
  impl->wcfg->state().setBackgroundGradient(gradient);
  impl->skin->setBackground(std::move(gradient));
}

void ClassicSkinSettings::on_bg_pattern_rbtn_clicked()
{
  impl->scfg->setBackground(impl->wcfg->state().getBackgroundPattern());
  impl->skin->setBackground(impl->wcfg->state().getBackgroundPattern());
}

void ClassicSkinSettings::on_bg_select_pattern_btn_clicked()
{
  auto file = QFileDialog::getOpenFileName(this,
                                           QString(),
                                           impl->last_path,
                                           tr("Images (*.png *.bmp *.jpg)"));
  if (file.isEmpty()) return;
  impl->last_path = file;
  QPixmap pxm(file);
  impl->scfg->setBackground(pxm);
  impl->wcfg->state().setBackgroundPattern(pxm);
  impl->skin->setBackground(std::move(pxm));
}

void ClassicSkinSettings::on_bg_pattern_stretch_clicked(bool checked)
{
  impl->scfg->setBackgroundStretch(checked);
  impl->skin->setBackgroundStretch(checked);
}

void ClassicSkinSettings::on_bg_per_element_cb_clicked(bool checked)
{
  impl->scfg->setBackgroundPerElement(checked);
  impl->skin->setBackgroundPerElement(checked);
}
