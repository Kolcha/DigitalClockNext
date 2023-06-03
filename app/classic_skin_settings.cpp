#include "classic_skin_settings.hpp"
#include "ui_classic_skin_settings.h"

#include <QSignalBlocker>

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
  {}
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
  impl->skin->formatter()->setFormat(impl->scfg->getTimeFormat());
  impl->skin->setOrientation(impl->scfg->getOrientation());
  impl->skin->setSpacing(impl->scfg->getSpacing());
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
