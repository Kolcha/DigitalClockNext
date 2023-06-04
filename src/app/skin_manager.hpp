#pragma once

#include "application_private.hpp"

#include "skin/classic_skin.hpp"

// skin manager is "integral part" of application
// it does "all magic" related to skin configuration
// so it needs an access to internal app structures
class SkinManagerImpl final : public SkinManager
{
  Q_OBJECT

public:
  explicit SkinManagerImpl(ApplicationPrivate* app, QObject* parent = nullptr);

  SkinPtr loadSkin(const QFont& font) const override;
  SkinPtr loadSkin(const QString& skin_name) const override;
  SkinPtr loadSkin(std::size_t i) const override;
  void configureSkin(const SkinPtr& skin, std::size_t i) const override;
  QStringList availableSkins() const override;

public slots:
  void findSkins() override;

private:
  using ClassicSkinPtr = std::shared_ptr<ClassicSkin>;
  ClassicSkinPtr loadLegacySkin(const QString& skin_path) const;

  void configureClassicSkin(const ClassicSkinPtr& skin, std::size_t i) const;

private:
  ApplicationPrivate* _app;

  enum class SkinType {
    Legacy,
  };

  struct LoaderInfo {
    SkinType type;
    QString path;
  };

  QHash<QString, LoaderInfo> _skins;
};
