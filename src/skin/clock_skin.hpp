#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <QDateTime>

#include "render/rendering.hpp"


class ClockRenderable : public LayoutSkinElement {
public:
  void setSeparatorsVisible(bool visible)
  {
    std::ranges::for_each(_separators, [=](const auto& s) { s->setVisible(visible); });
  }

  bool separatorsVisible() const
  {
    return std::ranges::all_of(_separators, [](const auto& s) { return s->isVisible(); });
  }

protected:
  using Separators = std::vector<std::shared_ptr<SkinElement>>;

  void setSeparators(Separators seps) noexcept { _separators = std::move(seps); }
  const Separators& separators() const noexcept { return _separators; }

private:
  Separators _separators;
};


class ClockSkin {
public:
  virtual ~ClockSkin() = default;

  virtual std::shared_ptr<ClockRenderable> process(const QDateTime& dt) = 0;
};
