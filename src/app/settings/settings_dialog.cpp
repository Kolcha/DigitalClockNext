/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QGraphicsEffect>

#include "app/application_private.hpp"
#include "classic_skin.hpp"

#include "app_global_settings.hpp"
#include "debug_settings.hpp"
#include "classic_skin_settings.hpp"
#include "time_format_settings.hpp"

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
  QString last_path;
  const int skin_tab_idx = 2;
  qsizetype skin_tab_cnt = 0;
  // contains objects names
  QSet<QString> global_tabs;
  QSet<QString> window_tabs;

  Impl(ApplicationPrivate* a, std::size_t i) noexcept
    : app(a)
    , idx(i)
    , wnd(a->window(i).get())
    , acfg(a->app_config().get())
    , wcfg(&a->app_config()->window(acfg->global().getConfigPerWindow() ? i : 0))
    , last_path(QDir::home().absoluteFilePath("clock_settings.dc5"))
  {}

  template<typename Method, typename... Args>
  void window(Method method, const Args&... args)
  {
    if (acfg->global().getConfigPerWindow()) {
      (*wnd.*method)(args...);
    } else {
      for (const auto& wnd : app->windows())
        (*wnd.*method)(args...);
    }
  }

  void applyEffect()
  {
    auto create_effect = [this]() {
      auto effect = new QGraphicsColorizeEffect;
      effect->setColor(wcfg->appearance().getColorizationColor());
      effect->setStrength(wcfg->appearance().getColorizationStrength());
      return effect;
    };
    if (acfg->global().getConfigPerWindow()) {
      wnd->setGraphicsEffect(create_effect());
    } else {
      for (const auto& wnd : app->windows())
        wnd->setGraphicsEffect(create_effect());
    }
  }

  template<typename Method, typename... Args>
  void updateEffect(Method modifier, const Args&... args)
  {
    if (acfg->global().getConfigPerWindow()) {
      (qobject_cast<QGraphicsColorizeEffect*>(wnd->graphicsEffect())->*modifier)(args...);
    } else {
      for (const auto& wnd : app->windows())
        (qobject_cast<QGraphicsColorizeEffect*>(wnd->graphicsEffect())->*modifier)(args...);
    }
  }
};

SettingsDialog::SettingsDialog(ApplicationPrivate* app, std::size_t idx, QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , impl(std::make_unique<Impl>(app, idx))
{
  ui->setupUi(this);
  ui->tabWidget->setTabVisible(ui->tabWidget->count() - 1, false);

  impl->window_tabs.insert(ui->general_tab->objectName());

  insertGlobalSettingsTab();
  insertDebugSettingsTab();

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
  ui->skin_cbox->setCurrentIndex(-1);   // if skin is available, next line will update the index
  ui->skin_cbox->setCurrentText(impl->wcfg->state().getLastUsedSkin());
  ui->is_separator_flashes->setChecked(appearance_cfg.getFlashingSeparator());
  ui->scaling_x_edit->setValue(appearance_cfg.getScaleFactorX());
  ui->scaling_y_edit->setValue(appearance_cfg.getScaleFactorY());
  ui->opacity_edit->setValue(qRound(appearance_cfg.getOpacity() * 100));
  ui->use_colorization->setChecked(appearance_cfg.getApplyColorization());
  ui->colorization_strength_edit->setValue(qRound(appearance_cfg.getColorizationStrength() * 100));
  const auto& general_cfg = impl->acfg->window(impl->idx).general();
  ui->use_time_zone->setChecked(!general_cfg.getShowLocalTime());
  ui->time_zone_edit->setCurrentText(tz_name(impl->acfg->window(impl->idx).state().getTimeZone()));

  ui->scaling_same_btn->setChecked(ui->scaling_x_edit->value() == ui->scaling_y_edit->value());
  on_scaling_same_btn_clicked(ui->scaling_same_btn->isChecked());

  updateSkinSettingsTab();

  if (!impl->acfg->global().getConfigPerWindow()) {
    auto wcfg = &impl->acfg->window(impl->idx);
    connect(this, &QDialog::accepted, wcfg, &WindowConfig::commit);
    connect(this, &QDialog::rejected, wcfg, &WindowConfig::discard);
  }
  connect(this, &QDialog::accepted, impl->wcfg, &WindowConfig::commit);
  connect(this, &QDialog::rejected, impl->wcfg, &WindowConfig::discard);
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::HideAppGlobalSettings()
{
  for (int i = 0; i < ui->tabWidget->count(); i++)
    if (impl->global_tabs.contains(ui->tabWidget->widget(i)->objectName()))
      ui->tabWidget->setTabVisible(i, false);
}

void SettingsDialog::HidePerWindowSettings()
{
  for (int i = 0; i < ui->tabWidget->count(); i++)
    if (impl->window_tabs.contains(ui->tabWidget->widget(i)->objectName()))
      ui->tabWidget->setTabVisible(i, false);
  ui->tabWidget->setCurrentIndex(0);
}

void SettingsDialog::insertSkinSettings(const QList<QWidget*>& tabs)
{
  for (qsizetype i = 0; i < impl->skin_tab_cnt; i++) {
    auto skin_tab = ui->tabWidget->widget(impl->skin_tab_idx);
    ui->tabWidget->removeTab(impl->skin_tab_idx);
    impl->window_tabs.remove(skin_tab->objectName());
    skin_tab->deleteLater();
  }

  for (auto iter = tabs.rbegin(); iter != tabs.rend(); ++iter) {
    Q_ASSERT(*iter);
    ui->tabWidget->insertTab(impl->skin_tab_idx, *iter, (*iter)->windowTitle());
    impl->window_tabs.insert((*iter)->objectName());
    // make sure that skin will be alive until settings tab destruction
    auto skin = impl->wnd->skin();
    connect(*iter, &QObject::destroyed, *iter, [skin]() { Q_UNUSED(skin); });
  }

  impl->skin_tab_cnt = tabs.size();
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
  impl->window(&ClockWindow::scale, arg1, impl->wcfg->appearance().getScaleFactorY());
  impl->wcfg->appearance().setScaleFactorX(arg1);
}

void SettingsDialog::on_scaling_y_edit_valueChanged(int arg1)
{
  impl->window(&ClockWindow::scale, impl->wcfg->appearance().getScaleFactorX(), arg1);
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
  impl->acfg->window(impl->idx).general().setShowLocalTime(!checked);
  applyTimeZoneSettings();
}

void SettingsDialog::on_time_zone_edit_activated(int index)
{
  impl->acfg->window(impl->idx).state().setTimeZone(ui->time_zone_edit->itemData(index).value<QTimeZone>());
  applyTimeZoneSettings();
}

void SettingsDialog::on_opacity_edit_valueChanged(int arg1)
{
  qreal opacity = arg1 / 100.;
  if (qFuzzyCompare(opacity, impl->wcfg->appearance().getOpacity()))
    return;
  impl->window(&ClockWindow::setWindowOpacity, opacity);
  impl->wcfg->appearance().setOpacity(opacity);
}

void SettingsDialog::on_use_colorization_clicked(bool checked)
{
  if (checked) {
    impl->applyEffect();
  } else {
    impl->window(&ClockWindow::setGraphicsEffect, nullptr);
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
  impl->updateEffect(&QGraphicsColorizeEffect::setColor, color);
  impl->wcfg->appearance().setColorizationColor(color);
}

void SettingsDialog::on_colorization_strength_edit_valueChanged(int arg1)
{
  qreal strength = arg1 / 100.;
  if (qFuzzyCompare(strength, impl->wcfg->appearance().getColorizationStrength()))
    return;
  impl->updateEffect(&QGraphicsColorizeEffect::setStrength, strength);
  impl->wcfg->appearance().setColorizationStrength(strength);
}

void SettingsDialog::on_export_btn_clicked()
{
  QString fname = QFileDialog::getSaveFileName(this,
                                               tr("Export Settings"),
                                               impl->last_path,
                                               tr("Clock Settings Files (*.dc5)"));
  if (fname.isEmpty()) return;
  impl->last_path = fname;
  impl->app->settings_manager()->exportSettings(fname);
}

void SettingsDialog::on_import_btn_clicked()
{
  QString fname = QFileDialog::getOpenFileName(this,
                                               tr("Import Settings"),
                                               impl->last_path,
                                               tr("Clock Settings Files (*.dc5)"));
  if (fname.isEmpty()) return;
  impl->last_path = fname;
  impl->app->settings_manager()->importSettings(fname);
}

void SettingsDialog::applySkin(std::shared_ptr<Skin> skin)
{
  impl->app->skin_manager()->configureSkin(skin, impl->idx);
  impl->window(&ClockWindow::setSkin, skin);
  updateSkinSettingsTab();
}

void SettingsDialog::applyFlashingSeparator(bool enable)
{
  impl->window(&ClockWindow::setSeparatorFlashes, enable);
}

void SettingsDialog::applyTimeZoneSettings()
{
  if (impl->acfg->window(impl->idx).general().getShowLocalTime())
    impl->wnd->setTimeZone(QDateTime::currentDateTime().timeZone());
  else
    impl->wnd->setTimeZone(impl->acfg->window(impl->idx).state().getTimeZone());
}

void SettingsDialog::insertGlobalSettingsTab()
{
  // TODO: disable tab if more than one dialog is opened
  auto w = new AppGlobalSettings(impl->app);
  impl->global_tabs.insert(w->objectName());
  connect(this, &QDialog::accepted, w, &AppGlobalSettings::commit);
  connect(this, &QDialog::rejected, w, &AppGlobalSettings::discard);
  ui->tabWidget->insertTab(0, w, tr("&App Global"));
}

void SettingsDialog::insertDebugSettingsTab()
{
  if (!impl->acfg->global().getEnableDebugOptions())
    return;
  // TODO: disable tab if more than one dialog is opened
  auto w = new DebugSettings(impl->app);
  impl->global_tabs.insert(w->objectName());
  connect(this, &QDialog::accepted, w, &DebugSettings::commit);
  connect(this, &QDialog::rejected, w, &DebugSettings::discard);
  ui->tabWidget->addTab(w, tr("&Debug"));
}

void SettingsDialog::updateSkinSettingsTab()
{
  SkinSettingsVisitor visitor(impl->app, impl->idx, this);
  impl->wnd->skin()->visit(visitor);
}

void SkinSettingsVisitor::visit(ClassicSkin* skin)
{
  _dlg->insertSkinSettings({
    new ClassicSkinSettings(skin, &_app->app_config()->window(_idx)),
    new TimeFormatSettings(skin, &_app->app_config()->window(_idx)),
  });
}

void SkinSettingsVisitor::visit(ErrorSkin* skin)
{
  Q_UNUSED(skin);
  _dlg->insertSkinSettings({});
}

void SkinSettingsVisitor::visit(ModernSkin* skin)
{
  Q_UNUSED(skin);
  _dlg->insertSkinSettings({});
}
