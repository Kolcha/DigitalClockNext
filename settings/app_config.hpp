#pragma once

#include <QObject>

#include <memory>
#include <ranges>
#include <vector>

#include <QString>
#include <QVariant>

#include "fr_appearance.hpp"
#include "fr_classic_skin.hpp"
#include "fr_window_state.hpp"
#include "settings.hpp"


class WindowConfig final : public QObject
{
  Q_OBJECT

  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;
  using ConfigStoragePtr = std::shared_ptr<ConfigStorageType>;

public:
  WindowConfig(const ConfigStoragePtr& storage, std::size_t i, QObject* parent = nullptr)
    : QObject(parent)
    , _appearance(std::make_unique<AppearanceConfig>(storage->client(t(i, "appearance"))))
    , _classic_skin(std::make_unique<ClassicSkinConfig>(storage->client(t(i, "classic_skin"))))
    , _state(std::make_unique<WindowState>(storage->client(t(i, "state"))))
  {}

  AppearanceConfig& appearance() const noexcept { return *_appearance; }
  ClassicSkinConfig& classicSkin() const noexcept { return *_classic_skin; }
  WindowState& state() const noexcept { return *_state; }

public slots:
  void commit()
  {
    _appearance->commit();
    _classic_skin->commit();
    _state->commit();
  }

  void discard()
  {
    _appearance->discard();
    _classic_skin->discard();
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
  {}

  // create objects on demand
  WindowConfig& window(std::size_t i)
  {
    if (_wnd_configs.size() < i + 1)
      _wnd_configs.resize(i + 1);

    auto& wnd_cfg = _wnd_configs[i];
    if (wnd_cfg)
      wnd_cfg = std::make_unique<WindowConfig>(_storage, i);

    return *wnd_cfg;
  }

public slots:
  void commit()
  {
    std::ranges::for_each(_wnd_configs, [](const auto& c) { c->commit(); });
  }

  void discard()
  {
    std::ranges::for_each(_wnd_configs, [](const auto& c) { c->discard(); });
  }

private:
  ConfigStoragePtr _storage;
  std::vector<std::unique_ptr<WindowConfig>> _wnd_configs;
};
