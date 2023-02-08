#pragma once

#include <memory>
#include <ranges>
#include <vector>

#include <QStringView>

#include "core/layout.hpp"
#include "core/renderable.hpp"


class ClockRenderable : public RenderableBase {
  using LayoutPtr = std::unique_ptr<Layout>;
  using Separators = std::vector<std::shared_ptr<Renderable>>;

public:
  explicit ClockRenderable(LayoutPtr layout, Separators separators)
  {
    setLayout(std::move(layout), std::move(separators));
  }

  QRectF rect() const override { return _layout->geometry(); }

  qreal advanceX() const override { return _layout->advanceX(); }
  qreal advanceY() const override { return _layout->advanceY(); }

  void setSeparatorsVisible(bool visible)
  {
    std::ranges::for_each(_separators, [=](const auto& s) { s->setVisible(visible); });
  }

  bool areSeparatorsVisible() const
  {
    return std::ranges::all_of(_separators, [](const auto& s) { return s->isVisible(); });
  }

protected:
  void setLayout(LayoutPtr layout, Separators separators)
  {
    Q_ASSERT(layout);
    _layout = std::move(layout);
    _separators = std::move(separators);
  }

  const auto& layout() const noexcept { return _layout; }
  const auto& separators() const noexcept { return _separators; }

private:
  LayoutPtr _layout;
  Separators _separators;
};


class ClockSkin {
public:
  virtual ~ClockSkin() = default;

  // TODO: replace with date/time type, string for now
  virtual std::shared_ptr<ClockRenderable> process(QStringView dt) = 0;
};
