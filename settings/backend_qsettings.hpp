#pragma once

#include "core/settings.hpp"

#include <QSettings>

template<typename T>
struct toValue<T, QVariant> {
  QVariant operator ()(const T& v) { return QVariant::fromValue(v); }
};

template<typename T>
struct fromValue<QVariant, T> {
  T operator ()(const QVariant& v) { return qvariant_cast<T>(v); }
};

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
      _settings->beginGroup(tag);
      const auto keys = _settings->allKeys();
      for (const auto& key : keys)
        cur_settings[key] = _settings->value(key);
      _settings->endGroup();
    }
    return all_settings;
  }

  void setValue(const QString& tag, const QString& k, const QVariant& v) override
  {
    _settings->setValue(backend_key(tag, k), v);
  }

  std::optional<QVariant> value(const QString& tag, const QString& k) const override
  {
    QVariant v = _settings->value(backend_key(tag, k));
    return v.isValid() ? std::optional(v) : std::nullopt;
  }

private:
  static QString backend_key(const QString& tag, const QString& key)
  {
    return QString("%1/%2").arg(tag, key);
  }

private:
  std::unique_ptr<QSettings> _settings;
};
