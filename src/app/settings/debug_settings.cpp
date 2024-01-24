/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "debug_settings.hpp"
#include "ui_debug_settings.h"

#include "app/application_private.hpp"

struct DebugSettings::Impl {
  ApplicationPrivate* app;
  DebugConfig& config;

  Impl(ApplicationPrivate* app) noexcept
    : app(app)
    , config(app->app_config()->debug())
  {}

  ~Impl()
  {
    app->applyDebugOptions();
  }

  bool dirty = false;

  void markDirty() noexcept { dirty = true; }
  bool resetDirty() noexcept { return std::exchange(dirty, false); }
};

DebugSettings::DebugSettings(ApplicationPrivate* app, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::DebugSettings)
  , impl(std::make_unique<Impl>(app))
{
  ui->setupUi(this);

  auto item_flags = impl->config.getItemDebugFlags();
  ui->draw_orig_r_i_cb->setChecked(item_flags & debug::DrawOriginalRect);
  ui->draw_origin_i_cb->setChecked(item_flags & debug::DrawOriginPoint);
  ui->draw_h_baseline_i_cb->setChecked(item_flags & debug::DrawHBaseline);
  ui->draw_v_baseline_i_cb->setChecked(item_flags & debug::DrawVBaseline);

  auto layout_flags = impl->config.getLayoutDebugFlags();
  ui->draw_orig_r_l_cb->setChecked(layout_flags & debug::DrawOriginalRect);
  ui->draw_origin_l_cb->setChecked(layout_flags & debug::DrawOriginPoint);
  ui->draw_h_baseline_l_cb->setChecked(layout_flags & debug::DrawHBaseline);
  ui->draw_v_baseline_l_cb->setChecked(layout_flags & debug::DrawVBaseline);

  const auto all_cboxes = findChildren<QCheckBox*>();
  for (const auto& cbox : all_cboxes)
    connect(cbox, &QCheckBox::clicked, impl->app, &ApplicationPrivate::applyDebugOptions);
}

DebugSettings::~DebugSettings()
{
  delete ui;
}

void DebugSettings::commit()
{
  if (!impl->resetDirty())
    return;

  impl->config.commit();
}

void DebugSettings::discard()
{
  if (!impl->resetDirty())
    return;

  impl->config.discard();
}

void DebugSettings::on_draw_orig_r_i_cb_clicked(bool checked)
{
  impl->config.setItemDebugFlags(impl->config.getItemDebugFlags().setFlag(debug::DrawOriginalRect, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_orig_r_l_cb_clicked(bool checked)
{
  impl->config.setLayoutDebugFlags(impl->config.getLayoutDebugFlags().setFlag(debug::DrawOriginalRect, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_origin_i_cb_clicked(bool checked)
{
  impl->config.setItemDebugFlags(impl->config.getItemDebugFlags().setFlag(debug::DrawOriginPoint, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_origin_l_cb_clicked(bool checked)
{
  impl->config.setLayoutDebugFlags(impl->config.getLayoutDebugFlags().setFlag(debug::DrawOriginPoint, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_h_baseline_i_cb_clicked(bool checked)
{
  impl->config.setItemDebugFlags(impl->config.getItemDebugFlags().setFlag(debug::DrawHBaseline, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_h_baseline_l_cb_clicked(bool checked)
{
  impl->config.setLayoutDebugFlags(impl->config.getLayoutDebugFlags().setFlag(debug::DrawHBaseline, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_v_baseline_i_cb_clicked(bool checked)
{
  impl->config.setItemDebugFlags(impl->config.getItemDebugFlags().setFlag(debug::DrawVBaseline, checked));
  impl->markDirty();
}

void DebugSettings::on_draw_v_baseline_l_cb_clicked(bool checked)
{
  impl->config.setLayoutDebugFlags(impl->config.getLayoutDebugFlags().setFlag(debug::DrawVBaseline, checked));
  impl->markDirty();
}
