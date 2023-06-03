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

private:
  Ui::ClassicSkinSettings* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
