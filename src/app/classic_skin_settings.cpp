#include "classic_skin_settings.hpp"
#include "ui_classic_skin_settings.h"

#include <QFileDialog>
#include <QColorDialog>
#include <QPixmapCache>
#include <QSignalBlocker>

#include <gradient_dialog.h>

#include "app/application_private.hpp"
#include "skin/classic_skin.hpp"

struct ClassicSkinSettings::Impl {
  ApplicationPrivate* app;
  std::size_t idx;
  ClockWindow* wnd;
  AppConfig* acfg;
  WindowConfig* wcfg;
  ClassicSkinConfig* scfg;

  std::shared_ptr<ClassicSkin> skin;

  Impl(ApplicationPrivate* a, std::size_t i) noexcept
    : app(a)
    , idx(i)
    , wnd(a->window(i).get())
    , acfg(a->app_config().get())
    , wcfg(&acfg->window(i))
    , scfg(&wcfg->classicSkin())
    , skin(std::dynamic_pointer_cast<ClassicSkin>(wnd->skin()))
    , last_path(QDir::homePath())
  {}

  QString last_path;
};

ClassicSkinSettings::ClassicSkinSettings(ApplicationPrivate* app, std::size_t idx, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::ClassicSkinSettings)
  , impl(std::make_unique<Impl>(app, idx))
{
  ui->setupUi(this);

  ui->orientation_cbox->addItem(tr("Horizontal"), Qt::Horizontal);
  ui->orientation_cbox->addItem(tr("Vertical"), Qt::Vertical);

  auto oidx = ui->orientation_cbox->findData(impl->scfg->getOrientation());
  ui->orientation_cbox->setCurrentIndex(oidx);

  QSignalBlocker _(ui->spacing_edit);
  ui->spacing_edit->setValue(impl->scfg->getSpacing());

  auto tfmt = impl->scfg->getTimeFormat();
  ui->allow_format_edit_cb->setChecked(ui->time_format_cbox->findText(tfmt) < 0);
  ui->time_format_cbox->setCurrentText(tfmt);

  auto tx = impl->scfg->getTexture();
  ui->texture_group->setChecked(tx.style() != Qt::NoBrush);
  ui->tx_solid_color_rbtn->setChecked(tx.style() == Qt::SolidPattern);
  ui->tx_gradient_rbtn->setChecked(tx.gradient() != nullptr);
  ui->tx_pattern_rbtn->setChecked(tx.style() == Qt::TexturePattern);
  ui->tx_per_element_cb->setChecked(impl->scfg->getTexturePerElement());

  auto bg = impl->scfg->getBackground();
  ui->background_group->setChecked(bg.style() != Qt::NoBrush);
  ui->bg_solid_color_rbtn->setChecked(bg.style() == Qt::SolidPattern);
  ui->bg_gradient_rbtn->setChecked(bg.gradient() != nullptr);
  ui->bg_pattern_rbtn->setChecked(bg.style() == Qt::TexturePattern);
  ui->bg_per_element_cb->setChecked(impl->scfg->getBackgroundPerElement());
}

ClassicSkinSettings::~ClassicSkinSettings()
{
  delete ui;
}

void ClassicSkinSettings::commitChanges()
{
  impl->scfg->commit();
}

void ClassicSkinSettings::discardChanges()
{
  impl->scfg->discard();
  impl->app->skin_manager()->configureSkin(impl->skin, impl->idx);
  impl->wnd->skinConfigured();
}

void ClassicSkinSettings::on_orientation_cbox_activated(int index)
{
  auto orientation = ui->orientation_cbox->itemData(index).value<Qt::Orientation>();
  impl->skin->setOrientation(orientation);
  impl->wnd->skinConfigured();
  impl->scfg->setOrientation(orientation);
}

void ClassicSkinSettings::on_spacing_edit_valueChanged(int arg1)
{
  impl->skin->setSpacing(arg1);
  impl->wnd->skinConfigured();
  impl->scfg->setSpacing(arg1);
}

void ClassicSkinSettings::on_time_format_cbox_textActivated(const QString& arg1)
{
  impl->skin->formatter()->setFormat(arg1);
  impl->wnd->skinConfigured();
  impl->scfg->setTimeFormat(arg1);
}

void ClassicSkinSettings::on_allow_format_edit_cb_toggled(bool checked)
{
  ui->time_format_cbox->setEditable(checked);
  ui->apply_format_btn->setEnabled(checked);
}

void ClassicSkinSettings::on_apply_format_btn_clicked()
{
  on_time_format_cbox_textActivated(ui->time_format_cbox->currentText());
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
  updateEffects();
}

void ClassicSkinSettings::on_tx_solid_color_rbtn_clicked()
{
  impl->scfg->setTexture(impl->wcfg->state().getTextureColor());
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_tx_gradient_rbtn_clicked()
{
  impl->scfg->setTexture(impl->wcfg->state().getTextureGradient());
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_tx_pattern_rbtn_clicked()
{
  impl->scfg->setTexture(impl->wcfg->state().getTexturePattern());
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_tx_per_element_cb_clicked(bool checked)
{
  impl->scfg->setTexturePerElement(checked);
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_bg_solid_color_rbtn_clicked()
{
  impl->scfg->setBackground(impl->wcfg->state().getBackgroundColor());
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_bg_gradient_rbtn_clicked()
{
  impl->scfg->setBackground(impl->wcfg->state().getBackgroundGradient());
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_bg_pattern_rbtn_clicked()
{
  impl->scfg->setBackground(impl->wcfg->state().getBackgroundPattern());
  updateEffects();
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
  updateEffects();
}

void ClassicSkinSettings::on_bg_per_element_cb_clicked(bool checked)
{
  impl->scfg->setBackgroundPerElement(checked);
  updateEffects();
}

void ClassicSkinSettings::updateEffects()
{
  QPixmapCache::clear();
  impl->app->skin_manager()->configureSkin(impl->skin, impl->idx);
  impl->wnd->skinConfigured();
}
