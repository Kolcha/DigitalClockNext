#pragma once

#include <QDialog>

#include <memory>

class ApplicationPrivate;
class ClockSkin;

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

public slots:
  void accept() override;
  void reject() override;

private slots:
  void on_font_rbtn_clicked();
  void on_skin_rbtn_clicked();

  void on_select_font_btn_clicked();
  void on_skin_cbox_activated(int index);

  void on_is_separator_flashes_clicked(bool checked);

private:
  void applySkin(std::shared_ptr<ClockSkin> skin);
  void applyFlashingSeparator(bool enable);

  void updateSkinSettingsTab();

private:
  Ui::SettingsDialog* ui;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
