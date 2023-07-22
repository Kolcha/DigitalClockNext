#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QButtonGroup>
#include <QFontDialog>

#include "app/application_private.hpp"
#include "app/classic_skin_settings.hpp"
#include "skin/classic_skin.hpp"

namespace {
using namespace Qt::Literals::StringLiterals;

QString tz_name(const QTimeZone& tz)
{
  return QString::fromLatin1(tz.id());
}

} // namespace

struct SettingsDialog::Impl {
  ApplicationPrivate* app;
  std::size_t idx;
  ClockWindow* wnd;
  AppConfig* acfg;
  WindowConfig* wcfg;

  std::shared_ptr<ClockSkin> last_skin;

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
  const auto& general_cfg = impl->wcfg->general();
  ui->use_time_zone->setChecked(!general_cfg.getShowLocalTime());
  ui->time_zone_edit->setCurrentText(tz_name(impl->wcfg->state().getTimeZone()));

  auto alignment_group = new QButtonGroup(this);
  alignment_group->addButton(ui->align_tl_btn, Qt::AlignTop | Qt::AlignLeft);
  alignment_group->addButton(ui->align_tc_btn, Qt::AlignTop | Qt::AlignHCenter);
  alignment_group->addButton(ui->align_tr_btn, Qt::AlignTop | Qt::AlignRight);
  alignment_group->addButton(ui->align_cl_btn, Qt::AlignVCenter | Qt::AlignLeft);
  alignment_group->addButton(ui->align_cc_btn, Qt::AlignVCenter | Qt::AlignHCenter);
  alignment_group->addButton(ui->align_cr_btn, Qt::AlignVCenter | Qt::AlignRight);
  alignment_group->addButton(ui->align_bl_btn, Qt::AlignBottom | Qt::AlignLeft);
  alignment_group->addButton(ui->align_bc_btn, Qt::AlignBottom | Qt::AlignHCenter);
  alignment_group->addButton(ui->align_br_btn, Qt::AlignBottom | Qt::AlignRight);
  auto alignment = impl->wcfg->general().getAlignment();
  ui->align_tl_btn->setChecked(alignment == (Qt::AlignTop | Qt::AlignLeft));
  ui->align_tc_btn->setChecked(alignment == (Qt::AlignTop | Qt::AlignHCenter));
  ui->align_tr_btn->setChecked(alignment == (Qt::AlignTop | Qt::AlignRight));
  ui->align_cl_btn->setChecked(alignment == (Qt::AlignVCenter | Qt::AlignLeft));
  ui->align_cc_btn->setChecked(alignment == (Qt::AlignVCenter | Qt::AlignHCenter));
  ui->align_cr_btn->setChecked(alignment == (Qt::AlignVCenter | Qt::AlignRight));
  ui->align_bl_btn->setChecked(alignment == (Qt::AlignBottom | Qt::AlignLeft));
  ui->align_bc_btn->setChecked(alignment == (Qt::AlignBottom | Qt::AlignHCenter));
  ui->align_br_btn->setChecked(alignment == (Qt::AlignBottom | Qt::AlignRight));
  connect(alignment_group, &QButtonGroup::idClicked,
          this, &SettingsDialog::onAlignmentButtonClicked);

  updateSkinSettingsTab();
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::accept()
{
  impl->wcfg->commit();
  QDialog::accept();
}

void SettingsDialog::reject()
{
  impl->wcfg->discard();
  impl->wnd->setSkin(impl->last_skin);
  applyFlashingSeparator(impl->wcfg->appearance().getFlashingSeparator());
  applyTimeZoneSettings();
  impl->wnd->scale(impl->wcfg->appearance().getScaleFactorX(), impl->wcfg->appearance().getScaleFactorY());
  impl->wnd->setAlignment(impl->wcfg->general().getAlignment());  // TODO: restore window state
  QDialog::reject();
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
  QFont fnt = QFontDialog::getFont(&ok, impl->wcfg->state().getTextSkinFont(), this);
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

void SettingsDialog::onAlignmentButtonClicked(int id)
{
  auto alignment = static_cast<Qt::Alignment>(id);
  impl->wnd->setAlignment(alignment);
  impl->wcfg->general().setAlignment(alignment);
}

void SettingsDialog::applySkin(std::shared_ptr<ClockSkin> skin)
{
  if (!skin) return;
  impl->app->skin_manager()->configureSkin(skin, impl->idx);
  impl->wnd->setSkin(std::move(skin));
  updateSkinSettingsTab();
}

void SettingsDialog::applyFlashingSeparator(bool enable)
{
  if (auto cskin = std::dynamic_pointer_cast<ClassicSkin>(impl->wnd->skin()))
    cskin->formatter()->setExtensionEnabled(u"legacy_skin"_s, enable);
  impl->wnd->setSeparatorFlashes(enable);
  if (!enable)
    impl->wnd->setSeparatorVisible(true);
}

void SettingsDialog::applyTimeZoneSettings()
{
  if (impl->wcfg->general().getShowLocalTime())
    impl->wnd->setTimeZone(QDateTime::currentDateTime().timeZone());
  else
    impl->wnd->setTimeZone(impl->wcfg->state().getTimeZone());
}

void SettingsDialog::updateSkinSettingsTab()
{
  constexpr auto skin_tab_pos = 1;
  const QString skin_tab_text = tr("Skin");

  if (ui->tabWidget->tabText(skin_tab_pos) == skin_tab_text) {
    auto skin_tab = ui->tabWidget->widget(skin_tab_pos);
    ui->tabWidget->removeTab(skin_tab_pos);
    delete skin_tab;
  }

  if (auto cskin = std::dynamic_pointer_cast<ClassicSkin>(impl->wnd->skin())) {
    auto w = new ClassicSkinSettings(impl->app, impl->idx);
    connect(this, &QDialog::accepted, w, &ClassicSkinSettings::commitChanges);
    connect(this, &QDialog::rejected, w, &ClassicSkinSettings::discardChanges);
    ui->tabWidget->insertTab(skin_tab_pos, w, skin_tab_text);
  }
}
