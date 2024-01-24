/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023-2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "layout_debug.hpp"
#include "skin_manager.hpp"

void ApplicationPrivate::initCore()
{
  _time_src = std::make_unique<TimeSource>();
  _skin_manager = std::make_unique<SkinManagerImpl>(this);
}

void ApplicationPrivate::initUpdater()
{
  if (!_app_config->global().getCheckForUpdates())
    return;

  _update_checker = std::make_unique<UpdateChecker>();
  _update_checker->setCheckForBeta(_app_config->global().getCheckForBetaVersion());

  auto save_last_update = [this]() {
    _app_state->global().setLastUpdateCheck(QDateTime::currentDateTime());
  };

  connect(_update_checker.get(), &UpdateChecker::newVersion, this, save_last_update);
  connect(_update_checker.get(), &UpdateChecker::upToDate, this, save_last_update);

  const auto& last_update = _app_state->global().getLastUpdateCheck();
  const auto update_period = _app_config->global().getUpdatePeriodDays();
  if (last_update.daysTo(QDateTime::currentDateTime()) >= update_period)
    _update_checker->checkForUpdates();
}

void ApplicationPrivate::applyDebugOptions()
{
  if (_app_config->global().getEnableDebugOptions()) {
    qputenv(debug::ItemDebugFlagsVar, QByteArray::number(_app_config->debug().getItemDebugFlags()));
    qputenv(debug::LayoutDebugFlagsVar, QByteArray::number(_app_config->debug().getLayoutDebugFlags()));
  } else {
    qunsetenv(debug::ItemDebugFlagsVar);
    qunsetenv(debug::LayoutDebugFlagsVar);
  }
}

void Application::initCore()
{
  _impl->applyDebugOptions();
  _impl->initCore();
}

void Application::initUpdater()
{
  _impl->initUpdater();

  if (!_impl->update_checker())
    return;

  connect(_impl->update_checker().get(), &UpdateChecker::newVersion, this, &Application::handleNewVersion);
  connect(_impl->update_checker().get(), &UpdateChecker::errorMessage, this, &Application::handleUpdateError);
}
