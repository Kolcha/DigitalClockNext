#pragma once

#include <QDialog>

#include <memory>

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

private:
  void applySkin(std::shared_ptr<Skin> skin);
  void applyFlashingSeparator(bool enable);
  void applyTimeZoneSettings();

  void insertGlobalSettingsTab();
  void updateSkinSettingsTab();

private:
  Ui::SettingsDialog* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
