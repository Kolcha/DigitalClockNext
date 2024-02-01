/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include <memory>

#include "skin_visitor.hpp"

class ApplicationPrivate;
class Skin;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(ApplicationPrivate* app,
                          std::size_t idx,
                          QWidget* parent = nullptr);
  ~SettingsDialog();

  void HideAppGlobalSettings();
  void HidePerWindowSettings();

  void insertSkinSettings(const QList<QWidget*>& tabs);

private slots:
  void on_font_rbtn_clicked();
  void on_skin_rbtn_clicked();

  void on_select_font_btn_clicked();
  void on_skin_cbox_activated(int index);

  void on_is_separator_flashes_clicked(bool checked);

  void on_scaling_x_edit_valueChanged(int arg1);
  void on_scaling_y_edit_valueChanged(int arg1);
  void on_scaling_same_btn_clicked(bool checked);

  void on_use_time_zone_clicked(bool checked);
  void on_time_zone_edit_activated(int index);

  void on_opacity_edit_valueChanged(int arg1);

  void on_use_colorization_clicked(bool checked);
  void on_select_colorization_color_clicked();
  void on_colorization_strength_edit_valueChanged(int arg1);

  void on_export_btn_clicked();
  void on_import_btn_clicked();

private:
  void applySkin(std::shared_ptr<Skin> skin);
  void applyFlashingSeparator(bool enable);
  void applyTimeZoneSettings();

  void insertGlobalSettingsTab();
  void insertDebugSettingsTab();
  void updateSkinSettingsTab();

private:
  Ui::SettingsDialog* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};

class SkinSettingsVisitor final : public SkinVisitor
{
public:
  SkinSettingsVisitor(ApplicationPrivate* app,
                      std::size_t idx,
                      SettingsDialog* dlg) noexcept
    : _app(app)
    , _idx(idx)
    , _dlg(dlg)
  {}

  void visit(ClassicSkin* skin) override;
  void visit(ErrorSkin* skin) override;

private:
  ApplicationPrivate* _app;
  std::size_t _idx;
  SettingsDialog* _dlg;
};
