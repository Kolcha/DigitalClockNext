#pragma once

#include <memory>

#include <QString>

#include "skin/classic_skin.hpp"

class ClassicSkinLoader {
public:
  virtual ~ClassicSkinLoader() = default;

  virtual std::unique_ptr<ClassicSkin> load(const QString& path) const = 0;
};
