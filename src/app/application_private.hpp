/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtCore/QObject>

#include <memory>
#include <vector>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSystemTrayIcon>

#include "app/clock_window.hpp"
#include "app/dialog_manager.hpp"
#include "app/update_checker.hpp"
#include "app/time_source.hpp"
#include "app_config.hpp"
#include "app_state.hpp"
#include "core/settings.hpp"
#include "platform/mouse_tracker.hpp"
#ifdef Q_OS_WINDOWS
#include "platform/win/stay_on_top_hacks.hpp"
#endif


class SkinManager : public QObject
{
  Q_OBJECT

public:
  using QObject::QObject;

  using SkinPtr = std::shared_ptr<Skin>;
  virtual SkinPtr loadSkin(const QFont& font) const = 0;
  virtual SkinPtr loadSkin(const QString& skin_name) const = 0;
  virtual SkinPtr loadSkin(std::size_t i) const = 0;
  virtual void configureSkin(const SkinPtr& skin, std::size_t i) const = 0;
  virtual QStringList availableSkins() const = 0;

public slots:
  virtual void findSkins() = 0;
};


class SettingsManager : public QObject
{
  Q_OBJECT

public:
  using QObject::QObject;

public slots:
  virtual void exportSettings(const QString& filename) = 0;
  virtual void importSettings(const QString& filename) = 0;

  virtual void confirmImport() = 0;
  virtual void discardImport() = 0;
};


class ApplicationPrivate : public QObject
{
  Q_OBJECT

  using DialogTag = quint64;

public:
  using QObject::QObject;
  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;

  // config
  void initConfig();

  inline const auto& app_config() const noexcept { return _app_config; }
  inline const auto& app_state() const noexcept { return _app_state; }
  inline const auto& config_storage() const noexcept { return _config_storage; }

  // tray
  void initTray();

  inline const auto& tray_icon() const noexcept { return _tray_icon; }
  inline const auto& tray_menu() const noexcept { return _tray_menu; }

  // core
  void initCore();
  void initUpdater();

  inline const auto& time_source() const noexcept { return _time_src; }
  inline const auto& skin_manager() const noexcept { return _skin_manager; }
  inline const auto& settings_manager() const noexcept { return _settings_manager; }
  inline const auto& mouse_tracker() const noexcept { return _mouse_tracker; }
  inline const auto& update_checker() const noexcept { return _update_checker; }

  // windows
  void initWindows(QScreen* primary_screen, QList<QScreen*> screens);
#ifdef Q_OS_WINDOWS
  void initStayOnTopHacks();
#endif
  void configureWindow(ClockWindow* wnd);
  inline void configureWindow(std::size_t i) { configureWindow(_windows[i].get()); }

  inline const auto& windows() const noexcept { return _windows; }
  inline const auto& window(std::size_t i) const noexcept { return _windows[i]; }

  std::size_t window_index(const ClockWindow* w) const noexcept;

  template<typename Dialog, typename... Args>
  Dialog* maybeCreateAndShowDialog(DialogTag tag, Args&&... args)
  {
    return _dialog_manager.maybeCreateAndShowDialog<Dialog>(tag, std::forward<Args>(args)...);
  }

public slots:
  void applyDebugOptions();

private:
  void createWindow(const QScreen* screen);

private:
  // config
  std::shared_ptr<ConfigStorageType> _config_storage;
  std::unique_ptr<AppConfig> _app_config;
  std::unique_ptr<AppState> _app_state;
  // tray
  std::unique_ptr<QSystemTrayIcon> _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;
  // clock-specific stuff
  DialogManager<DialogTag> _dialog_manager;
  std::vector<std::unique_ptr<ClockWindow>> _windows;
  std::unique_ptr<MouseTracker> _mouse_tracker;
  std::unique_ptr<TimeSource> _time_src;
  std::unique_ptr<SkinManager> _skin_manager;
  std::unique_ptr<SettingsManager> _settings_manager;
  // updater
  std::unique_ptr<UpdateChecker> _update_checker;
  // platform-specific hacks
#ifdef Q_OS_WINDOWS
  std::unique_ptr<WinStayOnToHacks> _win_stay_on_top_hacks;
#endif
};
