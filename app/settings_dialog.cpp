#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include <QFontDialog>

#include "app/application_private.hpp"
#include "app/classic_skin_settings.hpp"
#include "skin/classic_skin.hpp"

using namespace Qt::Literals::StringLiterals;

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

  const auto& appearance_cfg = impl->wcfg->appearance();
  ui->font_rbtn->setChecked(appearance_cfg.getUseFontInsteadOfSkin());
  ui->skin_rbtn->setChecked(!appearance_cfg.getUseFontInsteadOfSkin());
  ui->skin_cbox->addItems(impl->app->skin_manager()->availableSkins());
  ui->skin_cbox->setCurrentText(impl->wcfg->state().getLastUsedSkin());
  ui->is_separator_flashes->setChecked(appearance_cfg.getFlashingSeparator());

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
