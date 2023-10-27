#pragma once

#include <QApplication>

#include <memory>

#include <QDate>
#include <QUrl>
#include <QVersionNumber>

class ApplicationPrivate;

class Application final : public QApplication
{
  Q_OBJECT

public:
  // follows QApplication constructor
  Application(int& argc, char** argv);
  ~Application();

  void init();

private slots:
  void showSettingsDialog();
  void showAboutDialog();

  void handleNewVersion(QVersionNumber version, QDate date, QUrl link);
  void handleUpToDate();
  void handleUpdateError(QString error_msg);

private:
  void initConfig();
  void initTray();
  void initCore();
  void initUpdater();

  void createWindows();

private:
  std::unique_ptr<ApplicationPrivate> _impl;
};
