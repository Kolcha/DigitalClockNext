#pragma once

#include <QWidget>

#include <memory>

class ApplicationPrivate;

namespace Ui {
class ClassicSkinSettings;
}

class ClassicSkinSettings : public QWidget
{
  Q_OBJECT

public:
  explicit ClassicSkinSettings(ApplicationPrivate* app,
                               std::size_t idx,
                               QWidget* parent = nullptr);
  ~ClassicSkinSettings();

public slots:
  void commitChanges();
  void discardChanges();

private slots:
  void on_orientation_cbox_activated(int index);
  void on_spacing_edit_valueChanged(int arg1);
  void on_time_format_cbox_textActivated(const QString& arg1);
  void on_allow_format_edit_cb_toggled(bool checked);
  void on_apply_format_btn_clicked();

  void on_texture_group_clicked(bool checked);
  void on_tx_solid_color_rbtn_clicked();
  void on_tx_select_color_btn_clicked();
  void on_tx_gradient_rbtn_clicked();
  void on_tx_select_gradient_btn_clicked();
  void on_tx_pattern_rbtn_clicked();
  void on_tx_select_pattern_btn_clicked();
  void on_tx_per_element_cb_clicked(bool checked);

  void on_background_group_clicked(bool checked);
  void on_bg_solid_color_rbtn_clicked();
  void on_bg_select_color_btn_clicked();
  void on_bg_gradient_rbtn_clicked();
  void on_bg_select_gradient_btn_clicked();
  void on_bg_pattern_rbtn_clicked();
  void on_bg_select_pattern_btn_clicked();
  void on_bg_per_element_cb_clicked(bool checked);

private:
  void updateEffects();

private:
  Ui::ClassicSkinSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
