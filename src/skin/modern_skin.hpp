/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "skin.hpp"

#include <QDir>

class ModernSkin final : public Skin {
public:
  explicit ModernSkin(const QDir& skin_root);
  ~ModernSkin();

  std::shared_ptr<Resource> process(const QDateTime& dt) override;

  void setSeparatorAnimationEnabled(bool enabled) override;
  void animateSeparator() override;

  void visit(SkinVisitor& visitor) override { visitor.visit(this); }

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};
