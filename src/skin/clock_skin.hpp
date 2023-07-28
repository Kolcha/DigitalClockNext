#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <QDateTime>

#include "core/rendering.hpp"


class ClockRenderable : public LayoutSkinElement {
  // TODO: consider to move it to protected section
  using Separators = std::vector<std::shared_ptr<SkinElement>>;

public:
  void setSeparatorsVisible(bool visible)
  {
    std::ranges::for_each(_separators, [=](const auto& s) { s->setVisible(visible); });
  }

  // TODO: rename: drop "are"
  bool areSeparatorsVisible() const
  {
    return std::ranges::all_of(_separators, [](const auto& s) { return s->isVisible(); });
  }

protected:
  void setSeparators(Separators seps) noexcept { _separators = std::move(seps); }
  // TODO: drop auto, use concrete type instead
  const auto& separators() const noexcept { return _separators; }

private:
  Separators _separators;
};


class ClockSkin {
public:
  virtual ~ClockSkin() = default;

  virtual std::shared_ptr<ClockRenderable> process(const QDateTime& dt) = 0;
};
