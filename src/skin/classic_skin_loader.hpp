#pragma once

#include <memory>

#include <QVariantHash>

#include "skin/classic_skin.hpp"

class ClassicSkinLoader {
public:
  virtual ~ClassicSkinLoader() = default;

  const QVariantHash& meta() const noexcept { return _meta; }

  bool valid() const noexcept { return _is_valid; }

  virtual std::unique_ptr<ClassicSkin> skin() const = 0;

protected:
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
  QVariantHash _meta;
  bool _is_valid = false;
};
