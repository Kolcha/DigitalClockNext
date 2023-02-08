#pragma once

#include <memory>
#include <vector>

#include <QStringView>

#include "core/layout.hpp"
#include "core/renderable.hpp"

class ClockSkin {
public:
  virtual ~ClockSkin() = default;

  // TODO: replace with date/time type, string for now
  // TODO: make const, non-const for now just to save separators - not!
  // it should not rely on the same objects from factory! keep it non-const
  virtual std::unique_ptr<Layout> layout(QStringView dt) = 0;
  virtual std::vector<std::shared_ptr<Renderable>> separators() const = 0;
};
