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

#include "application.hpp"
#include "application_private.hpp"

#include <QDir>

#include "backend_qsettings.hpp"
#include "settings_manager.hpp"

void ApplicationPrivate::initConfig()
{
  using namespace Qt::Literals::StringLiterals;
  auto backend = std::make_shared<BackendQSettings>();
#ifndef Q_OS_MACOS
  QDir app_dir(QApplication::applicationDirPath());
  if (app_dir.exists(u"portable.txt"_s) || app_dir.exists(u".portable"_s))
    backend = std::make_shared<BackendQSettings>(app_dir.absoluteFilePath(u"settings.ini"_s));
#endif
  _app_state = std::make_unique<AppState>(backend);
  _config_storage = std::make_shared<ConfigStorageType>(std::move(backend));
  _app_config = std::make_unique<AppConfig>(_config_storage);
  _settings_manager = std::make_unique<SettingsManagerImpl>(this);
}

void Application::initConfig()
{
  _impl->initConfig();
}
