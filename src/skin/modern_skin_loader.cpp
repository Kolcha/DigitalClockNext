/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "modern_skin_loader.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

QString tryReadTitle(const QString& filename)
{
  QFile skin_file(filename);
  if (!skin_file.open(QIODevice::ReadOnly)) return {};

  auto js_doc = QJsonDocument::fromJson(skin_file.readAll());
  auto js_obj = js_doc.object();

  if (const auto v = js_obj["name"]; v.isString())
    return v.toString();

  return {};
}

ModernSkinLoader::ModernSkinLoader(const QString& path)
  : _path(path)
{
  QDir skin_root(path);
  if (skin_root.exists() && skin_root.exists("skin.json"))
    _title = tryReadTitle(skin_root.absoluteFilePath("skin.json"));
  _valid = !_title.isEmpty();
}

std::unique_ptr<ModernSkin> ModernSkinLoader::skin() const
{
  if (!_valid) return nullptr;
  return std::make_unique<ModernSkin>(QDir(_path));
}
