#pragma once

#include <memory>

#include <QChar>

#include "core/renderable.hpp"

class RenderableFactory {
public:
  virtual ~RenderableFactory() = default;

  virtual std::shared_ptr<Renderable> item(QChar ch) const = 0;
};
