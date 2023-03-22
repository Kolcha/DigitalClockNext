#pragma once

#include <QApplication>

#include <memory>

// forward declarations
class QMenu;
class QSystemTrayIcon;

class AppConfig;
class ConfigStorageType;

class Application final : public QApplication
{
  Q_OBJECT

public:
  // follows QApplication constructor
  Application(int& argc, char** argv);
  ~Application();

  void init();

private slots:
  // TODO: add optional 'screen' parameter
  // not set (aka from tray menu) - show on default screen
  void showSettingsDialog();
  void showAboutDialog();

private:
  void initConfig();
  void initTray();

  void createTrayIcon();
  void createTrayMenu();

private:
  // config
  std::shared_ptr<ConfigStorageType> _config_storage;
  std::unique_ptr<AppConfig> _app_config;
  // tray
  std::unique_ptr<QSystemTrayIcon> _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;
};
