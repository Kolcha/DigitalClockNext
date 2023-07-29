#pragma once

#include <QObject>

#include <algorithm>
#include <memory>
#include <vector>

#include <QString>
#include <QVariant>

#include "core/settings.hpp"
#include "sections/app_global.hpp"
#include "sections/appearance.hpp"
#include "sections/classic_skin.hpp"
#include "sections/general.hpp"
#include "sections/window_state.hpp"


class WindowConfig final : public QObject
{
  Q_OBJECT

  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;
  using ConfigStoragePtr = std::shared_ptr<ConfigStorageType>;

public:
  WindowConfig(const ConfigStoragePtr& storage, std::size_t i, QObject* parent = nullptr)
    : QObject(parent)
    , _appearance(std::make_unique<AppearanceConfig>(storage->client(t(i, "Appearance"))))
    , _classic_skin(std::make_unique<ClassicSkinConfig>(storage->client(t(i, "ClassicSkin"))))
    , _general(std::make_unique<GeneralConfig>(storage->client(t(i, "General"))))
    , _state(std::make_unique<WindowState>(storage->client(t(i, "State"))))
  {}

  AppearanceConfig& appearance() const noexcept { return *_appearance; }
  ClassicSkinConfig& classicSkin() const noexcept { return *_classic_skin; }
  GeneralConfig& general() const noexcept { return *_general; }
  WindowState& state() const noexcept { return *_state; }

public slots:
  void commit()
  {
    _appearance->commit();
    _classic_skin->commit();
    _general->commit();
    _state->commit();
  }

  void discard()
  {
    _appearance->discard();
    _classic_skin->discard();
    _general->discard();
    _state->discard();
  }

private:
  static QString t(std::size_t i, const char* tag)
  {
    return QString("%1%2/%3").arg(QLatin1String(prefix)).arg(i).arg(QLatin1String(tag));
  }

  static constexpr const char* const prefix = "Window";

  std::unique_ptr<AppearanceConfig> _appearance;
  std::unique_ptr<ClassicSkinConfig> _classic_skin;
  std::unique_ptr<GeneralConfig> _general;
  std::unique_ptr<WindowState> _state;
};


class AppConfig final : public QObject
{
  Q_OBJECT

  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;
  using ConfigStoragePtr = std::shared_ptr<ConfigStorageType>;

public:
  explicit AppConfig(ConfigStoragePtr storage, QObject* parent = nullptr)
    : QObject(parent)
    , _storage(std::move(storage))
    , _global_config(std::make_unique<AppGlobalConfig>(_storage->client("AppGlobal")))
  {}

  // create objects on demand
  WindowConfig& window(std::size_t i)
  {
    if (_wnd_configs.size() < i + 1)
      _wnd_configs.resize(i + 1);

    auto& wnd_cfg = _wnd_configs[i];
    if (!wnd_cfg)
      wnd_cfg = std::make_unique<WindowConfig>(_storage, i);

    return *wnd_cfg;
  }

  AppGlobalConfig& global() const noexcept { return *_global_config; }

public slots:
  void commit()
  {
    _global_config->commit();
    std::ranges::for_each(_wnd_configs, [](const auto& c) { c->commit(); });
  }

  void discard()
  {
    _global_config->discard();
    std::ranges::for_each(_wnd_configs, [](const auto& c) { c->discard(); });
  }

private:
  ConfigStoragePtr _storage;
  std::unique_ptr<AppGlobalConfig> _global_config;
  std::vector<std::unique_ptr<WindowConfig>> _wnd_configs;
};
