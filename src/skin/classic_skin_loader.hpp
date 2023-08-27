#pragma once

#include <memory>

#include <QVariantHash>

#include "skin/classic_skin.hpp"

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
