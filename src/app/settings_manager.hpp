/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2024  Nick Korotysh <nick.korotysh@gmail.com>

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

#include "application_private.hpp"

// settings manager is "integral part" of application
// it does "all magic" related to settings export/import
// so it needs an access to internal app structures
class SettingsManagerImpl final : public SettingsManager
{
  Q_OBJECT

public:
  explicit SettingsManagerImpl(ApplicationPrivate* app, QObject* parent = nullptr);

public slots:
  void exportSettings(const QString& filename) override;
  void importSettings(const QString& filename) override;

  void confirmImport() override;
  void discardImport() override;

private:
  void reconfigure();

private:
  ApplicationPrivate* _app;
  bool _settings_imported = false;
};
