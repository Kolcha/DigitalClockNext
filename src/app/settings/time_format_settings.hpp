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
class TimeFormatSettings;
}

class TimeFormatSettings : public QWidget
{
  Q_OBJECT

public:
  TimeFormatSettings(ApplicationPrivate* app, std::size_t idx,
                     QWidget* parent = nullptr);
  ~TimeFormatSettings();

private slots:
  void on_use_custom_format_toggled(bool checked);
  void on_format_edit_textEdited(const QString& arg1);
  void on_format_help_btn_clicked();
  void on_format_apply_btn_clicked();
  void on_custom_seps_edit_textEdited(const QString& arg1);

  void updateTimeFormat();

private:
  Ui::TimeFormatSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
