#pragma once

#include "render/renderable_base.hpp"

#include <memory>
#include <utility>

#include "render/layout_renderer.hpp"

class LayoutRenderable : public RenderableBase {
  using RendererPtr = std::shared_ptr<LayoutRenderer>;
  using LayoutPtr = std::unique_ptr<Layout>;

public:
  LayoutRenderable(RendererPtr renderer, LayoutPtr layout)
    : _renderer(std::move(renderer))
  {
    Q_ASSERT(_renderer);
    setLayout(std::move(layout));
  }

  QRectF rect() const override { return _layout->geometry(); }

  qreal advanceX() const override { return _layout->advanceX(); }
  qreal advanceY() const override { return _layout->advanceY(); }

protected:
  void renderImpl(QPainter* p) const final
  {
    _renderer->render(_layout.get(), p);
  }

  void setLayout(LayoutPtr layout)
  {
    Q_ASSERT(layout);
    _layout = std::move(layout);
  }

  const auto& layout() const noexcept { return _layout; }

private:
  RendererPtr _renderer;
  LayoutPtr _layout;
};
