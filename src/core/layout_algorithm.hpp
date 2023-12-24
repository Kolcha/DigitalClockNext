#pragma once

#include <memory>
#include <vector>

#include "glyph.hpp"

class LayoutAlgorithm {
public:
  using ContainerType = std::vector<std::shared_ptr<Glyph>>;

  virtual ~LayoutAlgorithm() = default;

  virtual std::pair<qreal, qreal> apply(const ContainerType& items) const = 0;
};
