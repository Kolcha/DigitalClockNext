#pragma once

#include "settings/core/settings.hpp"

#include <QSettings>

class BackendQSettings final : public ConfigBackend<QString, QString, QVariant> {
public:
  BackendQSettings()
    : _settings(std::make_unique<QSettings>())
  {}

  explicit BackendQSettings(const QString& filename)
    : _settings(std::make_unique<QSettings>(filename, QSettings::IniFormat))
  {}

  BackendQSettings(const QString& organization, const QString& application)
    : _settings(std::make_unique<QSettings>(organization, application))
  {}

  void load(const QString&) override {}
  void save(const QString&) override {}

  SettingsData allSettings() const override
  {
    SettingsData all_settings;
    const auto tags = _settings->childGroups();
    for (const auto& tag : tags) {
      auto& cur_settings = all_settings[tag];
      GroupGuard _(*_settings, tag);
      const auto keys = _settings->allKeys();
      for (const auto& key : keys)
        cur_settings[key] = _settings->value(key);
    }
    return all_settings;
  }

  void setValue(const QString& tag, const QString& k, const QVariant& v) override
  {
    GroupGuard _(*_settings, tag);
    _settings->setValue(k, v);
  }

  std::optional<QVariant> value(const QString& tag, const QString& k) const override
  {
    GroupGuard _(*_settings, tag);
    QVariant v = _settings->value(k);
    return v.isValid() ? std::optional(v) : std::nullopt;
  }

private:
  class GroupGuard final {
  public:
    GroupGuard(QSettings& s, QAnyStringView g)
      : _settings(s) { _settings.beginGroup(g); }
    ~GroupGuard() { _settings.endGroup(); }
  private:
    QSettings& _settings;
  };

private:
  std::unique_ptr<QSettings> _settings;
};
