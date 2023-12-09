/*
    Digital Clock - beautiful customizable clock with plugins
    Copyright (C) 2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "clock_skin.hpp"

#include <memory>

#include <QString>

#include "linear_layout.hpp"
#include "effects/composite.hpp"
#include "renderable_factory.hpp"

// TODO: consider to add setter for factory object -
// this will allow to change classic skins only with changing factory
class ClassicSkin : public ClockSkin {
public:
  explicit ClassicSkin(std::shared_ptr<RenderableFactory> provider)
    : _factory(std::move(provider))
    , _layout_alg(std::make_shared<LinearLayout>())
    , _item_effects(std::make_shared<CompositeEffect>())
    , _layout_effects(std::make_shared<CompositeEffect>())
    , _format(QLatin1String("hh:mm a"))
  {}

  std::shared_ptr<ClockRenderable> process(const QDateTime& dt) override;

  bool supportsSeparatorAnimation() const noexcept override
  {
    return _supports_separator_animation;
  }

  void setSeparatorAnimationEnabled(bool enabled) override
  {
    _animate_separator = enabled;
    _separator_visible = _separator_visible || !enabled;
  }

  void animateSeparator() override
  {
    if (!_animate_separator) return;
    _separator_visible = !_separator_visible;
  }

  void setSupportsCustomSeparator(bool supports) noexcept
  {
    _supports_custom_separator = supports;
  }

  void setSupportsSeparatorAnimation(bool supports) noexcept
  {
    _supports_separator_animation = supports;
  }

  /**
   * Sets custom @a separators.
   *
   * Characters count should match ':' count in format string.
   * ':' is used by default if there is not enough charaters for replacement.
   * Set an empty string to restore the default behavior.
   */
  void setCustomSeparators(QString separators) noexcept
  {
    _separators = std::move(separators);
  }

  bool supportsCustomSeparator() const noexcept
  {
    return _supports_custom_separator;
  }

  void setOrientation(Qt::Orientation orientation)
  {
    _layout_alg->setOrientation(orientation);
  }

  void setSpacing(qreal spacing)
  {
    _layout_alg->setSpacing(spacing);
  }

  void addItemEffect(std::shared_ptr<Effect> effect)
  {
    _item_effects->addEffect(std::move(effect));
  }

  void addLayoutEffect(std::shared_ptr<Effect> effect)
  {
    _layout_effects->addEffect(std::move(effect));
  }

  void clearItemEffects()
  {
    _item_effects->clearEffects();
  }

  void clearLayoutEffects()
  {
    _layout_effects->clearEffects();
  }

  void setFormat(QString format)
  {
    if (format.isEmpty() || format == _format)
      return;
    _format = std::move(format);
  }

  QString format() const { return _format; }

private:
  std::shared_ptr<RenderableFactory> _factory;
  std::shared_ptr<LinearLayout> _layout_alg;
  std::shared_ptr<CompositeEffect> _item_effects;
  std::shared_ptr<CompositeEffect> _layout_effects;
  // public properties
  bool _supports_custom_separator = false;
  bool _supports_separator_animation = false;
  // internal state
  bool _animate_separator = true;
  bool _separator_visible = true;
  QString _format;
  QString _separators;
};
