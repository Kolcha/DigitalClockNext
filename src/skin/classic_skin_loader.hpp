/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>

#include <QVariantHash>

#include "classic_skin.hpp"

class ClassicSkinLoader {
public:
  virtual ~ClassicSkinLoader() = default;

  const QString& title() const noexcept { return _title; }
  const QVariantHash& meta() const noexcept { return _meta; }

  bool valid() const noexcept { return _is_valid; }

  virtual std::unique_ptr<ClassicSkin> skin() const = 0;

protected:
  void setTitle(const QString& title) { _title = title; }
  void setMeta(const QVariantHash& meta) { _meta = meta; }

  void updateMeta(const QVariantHash& meta)
  {
    _meta.insert(meta);
  }

  void updateMeta(const QString& key, const QVariant& value)
  {
    _meta.insert(key, value);
  }

  void setValid(bool valid) noexcept { _is_valid = valid; }

private:
  QString _title;
  QVariantHash _meta;
  bool _is_valid = false;
};
