/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "modern_skin.hpp"

class ModernSkinLoader final {
public:
  explicit ModernSkinLoader(const QString& path);

  bool valid() const noexcept { return _valid; }

  QString title() const noexcept { return _title; }

  std::unique_ptr<ModernSkin> skin() const;

private:
  QString _path;
  QString _title;
  bool _valid = false;
};
