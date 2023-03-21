#pragma once

#include <QApplication>

#include <memory>

// forward declarations
class QMenu;
class QSystemTrayIcon;

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
  void initTray();

  void createTrayIcon();
  void createTrayMenu();

private:
  std::unique_ptr<QSystemTrayIcon> _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;
};
