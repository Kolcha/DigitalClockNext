#include "app_globalsettings.hpp"
#include "ui_app_globalsettings.h"

#include "app/application_private.hpp"

namespace {

int setIndexByValue(QComboBox* box, const QVariant& value)
{
  auto idx = box->findData(value);
  if (idx >= 0)
    box->setCurrentIndex(idx);
  return idx;
}

}

struct AppGlobalSettings::Impl {
  ApplicationPrivate* app;
  AppGlobalConfig& config;

  Impl(ApplicationPrivate* app) noexcept
    : app(app)
    , config(app->app_config()->global())
  {}

  bool dirty = false;

  void markDirty() noexcept { dirty = true; }
  bool resetDirty() noexcept { return std::exchange(dirty, false); }
};

AppGlobalSettings::AppGlobalSettings(ApplicationPrivate* app, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::AppGlobalSettings)
  , impl(std::make_unique<Impl>(app))
{
  ui->setupUi(this);

  ui->enable_autostart->setVisible(false);  // not implemented
  ui->enable_stay_on_top->setChecked(impl->config.getStayOnTop());
  ui->enable_transp_for_input->setChecked(impl->config.getTransparentForMouse());
  ui->enable_snap_to_edge->setChecked(impl->config.getSnapToEdge());
  ui->enable_autoupdate->setChecked(impl->config.getCheckForUpdates());
  ui->check_for_beta->setChecked(impl->config.getCheckForBetaVersion());

  QSignalBlocker _(ui->update_period_edit);
  ui->update_period_edit->addItem(tr("1 day"), 1);
  ui->update_period_edit->addItem(tr("1 week"), 7);
  ui->update_period_edit->addItem(tr("2 weeks"), 14);
  ui->update_period_edit->addItem(tr("1 month"), 30);
  setIndexByValue(ui->update_period_edit, impl->config.getUpdatePeriodDays());
}

AppGlobalSettings::~AppGlobalSettings()
{
  delete ui;
}

void AppGlobalSettings::commit()
{
  if (!impl->resetDirty())
    return;

  impl->config.commit();
}

void AppGlobalSettings::discard()
{
  if (!impl->resetDirty())
    return;

  impl->config.discard();

  impl->app->updater()->SetAutoupdate(impl->config.getCheckForUpdates());
  impl->app->updater()->SetUpdatePeriod(impl->config.getUpdatePeriodDays());
  impl->app->updater()->SetCheckForBeta(impl->config.getCheckForBetaVersion());
}

void AppGlobalSettings::on_enable_autostart_clicked(bool checked)
{
  Q_UNUSED(checked);
  // TODO: implement, consider multiple instances, create path-based "App ID"
}

void AppGlobalSettings::on_enable_stay_on_top_clicked(bool checked)
{
  impl->config.setStayOnTop(checked);
  impl->markDirty();
}

void AppGlobalSettings::on_enable_transp_for_input_clicked(bool checked)
{
  impl->config.setTransparentForMouse(checked);
  impl->markDirty();
}

void AppGlobalSettings::on_enable_snap_to_edge_clicked(bool checked)
{
  impl->config.setSnapToEdge(checked);
  impl->markDirty();
}

void AppGlobalSettings::on_enable_autoupdate_clicked(bool checked)
{
  impl->app->updater()->SetAutoupdate(checked);
  impl->config.setCheckForUpdates(checked);
  impl->markDirty();
}

void AppGlobalSettings::on_update_period_edit_activated(int index)
{
  int period = ui->update_period_edit->itemData(index).toInt();
  impl->app->updater()->SetUpdatePeriod(period);
  impl->config.setUpdatePeriodDays(period);
  impl->markDirty();
}

void AppGlobalSettings::on_check_for_beta_clicked(bool checked)
{
  impl->app->updater()->SetCheckForBeta(checked);
  impl->config.setCheckForBetaVersion(checked);
  impl->markDirty();
}
