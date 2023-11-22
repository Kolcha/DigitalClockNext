#pragma once

#include <memory>

#include <QDateTime>

#include "render/rendering.hpp"


class ClockRenderable : public LayoutSkinElement {
};


class ClockSkin {
public:
  virtual ~ClockSkin() = default;

  virtual std::shared_ptr<ClockRenderable> process(const QDateTime& dt) = 0;

  virtual bool supportsCustomSeparator() const noexcept = 0;
  virtual bool supportsSeparatorAnimation() const noexcept = 0;

  virtual void setSeparatorAnimationEnabled(bool enabled) = 0;
  inline void EnableSeparatorAnimation() { setSeparatorAnimationEnabled(true); }
  inline void DisableSeparatorAnimation() { setSeparatorAnimationEnabled(false); }

  virtual void animateSeparator() = 0;
};
