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
