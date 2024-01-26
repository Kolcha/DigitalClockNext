/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

class ApplicationPrivate;

namespace Ui {
class AppGlobalSettings;
}

class AppGlobalSettings : public QWidget
{
  Q_OBJECT

public:
  explicit AppGlobalSettings(ApplicationPrivate* app, QWidget* parent = nullptr);
  ~AppGlobalSettings();

public slots:
  void commit();
  void discard();

private slots:
  void on_enable_autostart_clicked(bool checked);
  void on_enable_stay_on_top_clicked(bool checked);
  void on_enable_transp_for_input_clicked(bool checked);
  void on_enable_snap_to_edge_clicked(bool checked);

  void on_enable_autoupdate_clicked(bool checked);
  void on_update_period_edit_activated(int index);
  void on_check_for_beta_clicked(bool checked);

  void on_enable_multiwindow_clicked(bool checked);
  void on_wnd_count_edit_valueChanged(int arg1);
  void on_use_same_appearance_clicked(bool checked);

  void on_enable_debug_options_clicked(bool checked);

private:
  Ui::AppGlobalSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
