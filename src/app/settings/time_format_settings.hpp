/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

class ClassicSkin;
class WindowConfig;

namespace Ui {
class TimeFormatSettings;
}

class TimeFormatSettings : public QWidget
{
  Q_OBJECT

public:
  TimeFormatSettings(ClassicSkin* skin, WindowConfig* wcfg,
                     QWidget* parent = nullptr);
  ~TimeFormatSettings();

private slots:
  void on_smaller_seconds_clicked(bool checked);
  void on_seconds_scale_factor_edit_valueChanged(int arg1);
  void on_use_custom_format_toggled(bool checked);
  void on_format_edit_textChanged(const QString& arg1);
  void on_format_edit_textEdited(const QString& arg1);
  void on_format_help_btn_clicked();
  void on_format_apply_btn_clicked();
  void on_custom_seps_edit_textEdited(const QString& arg1);
  void on_layout_cfg_edit_textEdited(const QString& arg1);
  void on_layout_cfg_help_btn_clicked();

  void updateTimeFormat();

private:
  Ui::TimeFormatSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
