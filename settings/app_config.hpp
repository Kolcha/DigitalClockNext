#pragma once

#include <QObject>

#include <memory>

#include <QString>
#include <QVariant>

#include "fr_appearance.hpp"
#include "fr_classic_skin.hpp"
#include "fr_state.hpp"
#include "settings.hpp"

using namespace Qt::Literals::StringLiterals;

class AppConfig : public QObject {
  Q_OBJECT

  using ConfigStorageType = ConfigStorage<QString, QString, QVariant>;
  using ConfigStoragePtr = std::shared_ptr<ConfigStorageType>;

public:
  explicit AppConfig(ConfigStoragePtr storage, QObject* parent = nullptr)
    : QObject(parent)
    , _appearance(std::make_unique<AppearanceConfig>(storage->client(u"appearance"_s)))
    , _classic_skin(std::make_unique<ClassicSkinConfig>(storage->client(u"classic_skin"_s)))
    , _state(std::make_unique<AppState>(storage->client(u"state"_s)))
  {}

  AppearanceConfig& Appearance() const noexcept { return *_appearance; }
  ClassicSkinConfig& ClassicSkin() const noexcept { return *_classic_skin; }
  AppState& State() const noexcept { return *_state; }

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
  std::unique_ptr<AppearanceConfig> _appearance;
  std::unique_ptr<ClassicSkinConfig> _classic_skin;
  std::unique_ptr<AppState> _state;
};
