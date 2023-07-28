#pragma once

#include "skin/renderable_factory.hpp"

#include <QFont>

#include "render/char_renderable.hpp"

class QCharRenderableFactory final : public RenderableFactory {
public:
  explicit QCharRenderableFactory(const QFont& fnt)
    : _fnt(fnt)
  {}

public:
  const QFont& font() const noexcept { return _fnt; }

  std::shared_ptr<SkinResource> item(QChar ch) const override
  {
    return std::make_shared<QCharRenderable>(ch, _fnt);
  }

private:
  QFont _fnt;
};
