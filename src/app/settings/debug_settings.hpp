/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

class ApplicationPrivate;

namespace Ui {
class DebugSettings;
}

class DebugSettings : public QWidget
{
  Q_OBJECT

public:
  explicit DebugSettings(ApplicationPrivate* app, QWidget* parent = nullptr);
  ~DebugSettings();

public slots:
  void commit();
  void discard();

private slots:
  void on_draw_orig_r_i_cb_clicked(bool checked);
  void on_draw_orig_r_l_cb_clicked(bool checked);
  void on_draw_origin_i_cb_clicked(bool checked);
  void on_draw_origin_l_cb_clicked(bool checked);
  void on_draw_h_baseline_i_cb_clicked(bool checked);
  void on_draw_h_baseline_l_cb_clicked(bool checked);
  void on_draw_v_baseline_i_cb_clicked(bool checked);
  void on_draw_v_baseline_l_cb_clicked(bool checked);

private:
  Ui::DebugSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
