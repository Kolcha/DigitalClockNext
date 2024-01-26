/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
