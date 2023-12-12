/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QColorDialog>
#include <QFontDialog>
#include <QGraphicsEffect>

#include "app/application_private.hpp"
#include "classic_skin.hpp"

#include "app_globalsettings.hpp"
#include "classic_skin_settings.hpp"

namespace {
using namespace Qt::Literals::StringLiterals;

QString tz_name(const QTimeZone& tz)
{
  return QString::fromLatin1(tz.id());
}

QFont actual_font(const QFont& fnt)
{
  QFont res_font = fnt;
  QFontInfo fi(fnt);
  res_font.setFamily(fi.family());
  res_font.setPointSizeF(fi.pointSizeF());
  res_font.setStyle(fi.style());
  res_font.setStyleHint(fi.styleHint());
  return res_font;
}

} // namespace

struct SettingsDialog::Impl {
  ApplicationPrivate* app;
  std::size_t idx;
  ClockWindow* wnd;
  AppConfig* acfg;
  WindowConfig* wcfg;

  std::shared_ptr<Skin> last_skin;

  Impl(ApplicationPrivate* a, std::size_t i) noexcept
    : app(a)
    , idx(i)
    , wnd(a->window(i).get())
    , acfg(a->app_config().get())
    , wcfg(&a->app_config()->window(i))
    , last_skin(wnd->skin())
  {}
};

SettingsDialog::SettingsDialog(ApplicationPrivate* app, std::size_t idx, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , impl(std::make_unique<Impl>(app, idx))
{
  ui->setupUi(this);

  insertGlobalSettingsTab();

  auto now = QDateTime::currentDateTimeUtc();
  for (const auto& tz_id : QTimeZone::availableTimeZoneIds()) {
    QTimeZone tz(tz_id);
    ui->time_zone_edit->addItem(tz_name(tz), QVariant::fromValue(tz));
    auto tooltip = QString("%1 (%2)").arg(
                     tz.displayName(now, QTimeZone::LongName),
                     tz.displayName(now, QTimeZone::OffsetName));
    ui->time_zone_edit->setItemData(ui->time_zone_edit->count() - 1, tooltip, Qt::ToolTipRole);
  }

  const auto& appearance_cfg = impl->wcfg->appearance();
  ui->font_rbtn->setChecked(appearance_cfg.getUseFontInsteadOfSkin());
  ui->skin_rbtn->setChecked(!appearance_cfg.getUseFontInsteadOfSkin());
  ui->skin_cbox->addItems(impl->app->skin_manager()->availableSkins());
  ui->skin_cbox->setCurrentText(impl->wcfg->state().getLastUsedSkin());
  ui->is_separator_flashes->setChecked(appearance_cfg.getFlashingSeparator());
  ui->scaling_x_edit->setValue(appearance_cfg.getScaleFactorX());
  ui->scaling_y_edit->setValue(appearance_cfg.getScaleFactorY());
  ui->opacity_edit->setValue(qRound(appearance_cfg.getOpacity() * 100));
  ui->use_colorization->setChecked(appearance_cfg.getApplyColorization());
  ui->colorization_strength_edit->setValue(qRound(appearance_cfg.getColorizationStrength() * 100));
  const auto& general_cfg = impl->wcfg->general();
  ui->use_time_zone->setChecked(!general_cfg.getShowLocalTime());
  ui->time_zone_edit->setCurrentText(tz_name(impl->wcfg->state().getTimeZone()));

  ui->scaling_same_btn->setChecked(ui->scaling_x_edit->value() == ui->scaling_y_edit->value());
  on_scaling_same_btn_clicked(ui->scaling_same_btn->isChecked());

  updateSkinSettingsTab();
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::on_font_rbtn_clicked()
{
  auto skin = impl->app->skin_manager()->loadSkin(impl->wcfg->state().getTextSkinFont());
  applySkin(std::move(skin));
  impl->wcfg->appearance().setUseFontInsteadOfSkin(true);
}

void SettingsDialog::on_skin_rbtn_clicked()
{
  auto skin = impl->app->skin_manager()->loadSkin(impl->wcfg->state().getLastUsedSkin());
  applySkin(std::move(skin));
  impl->wcfg->appearance().setUseFontInsteadOfSkin(false);
}

void SettingsDialog::on_select_font_btn_clicked()
{
  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, actual_font(impl->wcfg->state().getTextSkinFont()), this);
  if (!ok) return;
  auto skin = impl->app->skin_manager()->loadSkin(fnt);
  applySkin(std::move(skin));
  impl->wcfg->state().setTextSkinFont(fnt);
}

void SettingsDialog::on_skin_cbox_activated(int index)
{
  QString skin_name = ui->skin_cbox->itemText(index);
  auto skin = impl->app->skin_manager()->loadSkin(skin_name);
  applySkin(std::move(skin));
  impl->wcfg->state().setLastUsedSkin(skin_name);
}

void SettingsDialog::on_is_separator_flashes_clicked(bool checked)
{
  applyFlashingSeparator(checked);
  impl->wcfg->appearance().setFlashingSeparator(checked);
}

void SettingsDialog::on_scaling_x_edit_valueChanged(int arg1)
{
  impl->wnd->scale(arg1, impl->wcfg->appearance().getScaleFactorY());
  impl->wcfg->appearance().setScaleFactorX(arg1);
}

void SettingsDialog::on_scaling_y_edit_valueChanged(int arg1)
{
  impl->wnd->scale(impl->wcfg->appearance().getScaleFactorX(), arg1);
  impl->wcfg->appearance().setScaleFactorY(arg1);
}

void SettingsDialog::on_scaling_same_btn_clicked(bool checked)
{
  if (checked) {
    ui->scaling_y_edit->setValue(ui->scaling_x_edit->value());
    connect(ui->scaling_x_edit, &QSpinBox::valueChanged, ui->scaling_y_edit, &QSpinBox::setValue);
    connect(ui->scaling_y_edit, &QSpinBox::valueChanged, ui->scaling_x_edit, &QSpinBox::setValue);
  } else {
    disconnect(ui->scaling_x_edit, &QSpinBox::valueChanged, ui->scaling_y_edit, &QSpinBox::setValue);
    disconnect(ui->scaling_y_edit, &QSpinBox::valueChanged, ui->scaling_x_edit, &QSpinBox::setValue);
  }
}

void SettingsDialog::on_use_time_zone_clicked(bool checked)
{
  impl->wcfg->general().setShowLocalTime(!checked);
  applyTimeZoneSettings();
}

void SettingsDialog::on_time_zone_edit_activated(int index)
{
  impl->wcfg->state().setTimeZone(ui->time_zone_edit->itemData(index).value<QTimeZone>());
  applyTimeZoneSettings();
}

void SettingsDialog::on_opacity_edit_valueChanged(int arg1)
{
  qreal opacity = arg1 / 100.;
  if (qFuzzyCompare(opacity, impl->wcfg->appearance().getOpacity()))
    return;
  impl->wnd->setWindowOpacity(opacity);
  impl->wcfg->appearance().setOpacity(opacity);
}

void SettingsDialog::on_use_colorization_clicked(bool checked)
{
  if (checked) {
    auto effect = new QGraphicsColorizeEffect;
    effect->setColor(impl->wcfg->appearance().getColorizationColor());
    effect->setStrength(impl->wcfg->appearance().getColorizationStrength());
    impl->wnd->setGraphicsEffect(effect);
  } else {
    impl->wnd->setGraphicsEffect(nullptr);
  }
  impl->wcfg->appearance().setApplyColorization(checked);
}

void SettingsDialog::on_select_colorization_color_clicked()
{
  auto color = QColorDialog::getColor(impl->wcfg->appearance().getColorizationColor(),
                                      this,
                                      QString(),
                                      QColorDialog::ShowAlphaChannel);
  if (!color.isValid()) return;
  qobject_cast<QGraphicsColorizeEffect*>(impl->wnd->graphicsEffect())->setColor(color);
  impl->wcfg->appearance().setColorizationColor(color);
}

void SettingsDialog::on_colorization_strength_edit_valueChanged(int arg1)
{
  qreal strength = arg1 / 100.;
  if (qFuzzyCompare(strength, impl->wcfg->appearance().getColorizationStrength()))
    return;
  qobject_cast<QGraphicsColorizeEffect*>(impl->wnd->graphicsEffect())->setStrength(strength);
  impl->wcfg->appearance().setColorizationStrength(strength);
}

void SettingsDialog::applySkin(std::shared_ptr<Skin> skin)
{
  impl->app->skin_manager()->configureSkin(skin, impl->idx);
  impl->wnd->setSkin(std::move(skin));
  updateSkinSettingsTab();
}

void SettingsDialog::applyFlashingSeparator(bool enable)
{
  impl->wnd->setSeparatorFlashes(enable);
}

void SettingsDialog::applyTimeZoneSettings()
{
  if (impl->wcfg->general().getShowLocalTime())
    impl->wnd->setTimeZone(QDateTime::currentDateTime().timeZone());
  else
    impl->wnd->setTimeZone(impl->wcfg->state().getTimeZone());
}

void SettingsDialog::insertGlobalSettingsTab()
{
  // TODO: disable tab if more than one dialog is opened
  auto w = new AppGlobalSettings(impl->app);
  connect(this, &QDialog::accepted, w, &AppGlobalSettings::commit);
  connect(this, &QDialog::rejected, w, &AppGlobalSettings::discard);
  ui->tabWidget->insertTab(0, w, tr("&App Global"));
}

void SettingsDialog::updateSkinSettingsTab()
{
  constexpr auto skin_tab_pos = 2;
  const QString skin_tab_text = tr("Skin");

  if (ui->tabWidget->tabText(skin_tab_pos) == skin_tab_text) {
    auto skin_tab = ui->tabWidget->widget(skin_tab_pos);
    ui->tabWidget->removeTab(skin_tab_pos);
    delete skin_tab;
  }

  if (auto cskin = std::dynamic_pointer_cast<ClassicSkin>(impl->wnd->skin())) {
    auto w = new ClassicSkinSettings(impl->app, impl->idx);
    ui->tabWidget->insertTab(skin_tab_pos, w, skin_tab_text);
  }
}
