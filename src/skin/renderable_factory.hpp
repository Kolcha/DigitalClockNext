#pragma once

#include <memory>

#include <QChar>

#include "render/skin_resource.hpp"

class RenderableFactory {
public:
  virtual ~RenderableFactory() = default;

  virtual std::shared_ptr<SkinResource> item(QChar ch) const = 0;
};
