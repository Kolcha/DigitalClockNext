/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "application_private.hpp"

#include "skin_visitor.hpp"

class SkinConfigurator final : public SkinVisitor
{
public:
  explicit SkinConfigurator(const WindowConfig& wnd_cfg) noexcept
    : _wnd_config(wnd_cfg)
  {}

  void visit(ClassicSkin* skin) override;
  void visit(ErrorSkin* skin) noexcept override { Q_UNUSED(skin); }

private:
  const WindowConfig& _wnd_config;
};

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
