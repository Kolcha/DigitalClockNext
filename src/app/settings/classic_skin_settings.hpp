/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include <memory>

class ClassicSkin;
class WindowConfig;

namespace Ui {
class ClassicSkinSettings;
}

class ClassicSkinSettings : public QWidget
{
  Q_OBJECT

public:
  ClassicSkinSettings(ClassicSkin* skin, WindowConfig* wcfg,
                      QWidget* parent = nullptr);
  ~ClassicSkinSettings();

private slots:
  void on_orientation_cbox_activated(int index);
  void on_spacing_edit_valueChanged(int arg1);
  void on_ignore_advance_x_clicked(bool checked);
  void on_ignore_advance_y_clicked(bool checked);

  void on_texture_group_clicked(bool checked);
  void on_tx_solid_color_rbtn_clicked();
  void on_tx_select_color_btn_clicked();
  void on_tx_gradient_rbtn_clicked();
  void on_tx_select_gradient_btn_clicked();
  void on_tx_pattern_rbtn_clicked();
  void on_tx_select_pattern_btn_clicked();
  void on_tx_pattern_stretch_clicked(bool checked);
  void on_tx_per_element_cb_clicked(bool checked);

  void on_background_group_clicked(bool checked);
  void on_bg_solid_color_rbtn_clicked();
  void on_bg_select_color_btn_clicked();
  void on_bg_gradient_rbtn_clicked();
  void on_bg_select_gradient_btn_clicked();
  void on_bg_pattern_rbtn_clicked();
  void on_bg_select_pattern_btn_clicked();
  void on_bg_pattern_stretch_clicked(bool checked);
  void on_bg_per_element_cb_clicked(bool checked);

private:
  Ui::ClassicSkinSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
