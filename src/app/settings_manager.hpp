/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
