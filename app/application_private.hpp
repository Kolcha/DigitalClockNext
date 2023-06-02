#pragma once

#include <QtCore/QObject>

#include <memory>
#include <vector>

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSystemTrayIcon>

#include "app/clock_window.hpp"
#include "clock/time_source.hpp"
#include "settings/app_config.hpp"
#include "settings/settings.hpp"


class SkinManager : public QObject
{
  Q_OBJECT

public:
  using QObject::QObject;

  using SkinPtr = std::shared_ptr<ClockSkin>;
  virtual SkinPtr loadSkin(const QFont& font) const = 0;
  virtual SkinPtr loadSkin(const QString& skin_name) const = 0;
  virtual SkinPtr loadSkin(std::size_t i) const = 0;
  virtual void configureSkin(const SkinPtr& skin, std::size_t i) const = 0;
};


class ApplicationPrivate : public QObject
{
  Q_OBJECT

public:
  using QObject::QObject;

  // config
  void initConfig();

  inline const auto& app_config() const noexcept { return _app_config; }

  // tray
  void initTray();

  inline const auto& tray_icon() const noexcept { return _tray_icon; }
  inline const auto& tray_menu() const noexcept { return _tray_menu; }

  // core
  void initCore();

  inline const auto& time_source() const noexcept { return _time_src; }
  inline const auto& skin_manager() const noexcept { return _skin_manager; }

  // windows
  void initWindows(QScreen* primary_screen, QList<QScreen*> screens);

  inline const auto& windows() const noexcept { return _windows; }
  inline const auto& window(std::size_t i) const noexcept { return _windows[i]; }

  std::size_t window_index(const ClockWindow* w) const noexcept;

private:
  void createWindow(const QScreen* screen);

private:
  // config
  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;
  std::shared_ptr<ConfigStorageType> _config_storage;
  std::unique_ptr<AppConfig> _app_config;
  // tray
  std::unique_ptr<QSystemTrayIcon> _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;
  // clock-specific stuff
  std::vector<std::unique_ptr<ClockWindow>> _windows;
  std::unique_ptr<TimeSource> _time_src;
  std::unique_ptr<SkinManager> _skin_manager;
};
