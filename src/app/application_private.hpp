/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
#include "app/updater/updater.hpp"
#include "app/time_source.hpp"
#include "app_config.hpp"
#include "app_state.hpp"
#include "core/settings.hpp"


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
  virtual QStringList availableSkins() const = 0;

public slots:
  virtual void findSkins() = 0;
};


class ApplicationPrivate : public QObject
{
  Q_OBJECT

  using DialogTag = quint64;

public:
  using QObject::QObject;

  // config
  void initConfig();

  inline const auto& app_config() const noexcept { return _app_config; }
  inline const auto& app_state() const noexcept { return _app_state; }

  // tray
  void initTray();

  inline const auto& tray_icon() const noexcept { return _tray_icon; }
  inline const auto& tray_menu() const noexcept { return _tray_menu; }

  // core
  void initCore();
  void initUpdater();

  inline const auto& time_source() const noexcept { return _time_src; }
  inline const auto& skin_manager() const noexcept { return _skin_manager; }
  inline const auto& updater() const noexcept { return _updater; }

  // windows
  void initWindows(QScreen* primary_screen, QList<QScreen*> screens);
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

private:
  void createWindow(const QScreen* screen);

private:
  // config
  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;
  std::shared_ptr<ConfigStorageType> _config_storage;
  std::unique_ptr<AppConfig> _app_config;
  std::unique_ptr<AppState> _app_state;
  // tray
  std::unique_ptr<QSystemTrayIcon> _tray_icon;
  std::unique_ptr<QMenu> _tray_menu;
  // clock-specific stuff
  DialogManager<DialogTag> _dialog_manager;
  std::vector<std::unique_ptr<ClockWindow>> _windows;
  std::unique_ptr<TimeSource> _time_src;
  std::unique_ptr<SkinManager> _skin_manager;
  // updater
  using Updater = digital_clock::core::Updater;
  std::unique_ptr<Updater> _updater;
};
