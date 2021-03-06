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

#include "skin.hpp"

#include <memory>

#include <QBrush>
#include <QString>

#include "linear_layout.hpp"
#include "resource.hpp"

// TODO: consider to add setter for factory object -
// this will allow to change classic skins only with changing factory
class ClassicSkin final : public Skin {
public:
  explicit ClassicSkin(std::shared_ptr<ResourceFactory> factory)
    : _factory(std::move(factory))
    , _layout_alg(std::make_shared<LinearLayout>())
    , _format(QLatin1String("hh:mm a"))
  {
    updateConfigHash();   // to set default value
  }

  std::shared_ptr<Glyph> process(const QDateTime& dt) override;

  void setSeparatorAnimationEnabled(bool enabled) override
  {
    _animate_separator = enabled;
    _separator_visible = _separator_visible || !enabled;
    handleConfigChange();
  }

  void animateSeparator() noexcept override
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
  void setCustomSeparators(QString separators)
  {
    _separators = std::move(separators);
    handleConfigChange();
  }

  bool supportsCustomSeparator() const noexcept
  {
    return _supports_custom_separator;
  }

  bool supportsSeparatorAnimation() const noexcept
  {
    return _supports_separator_animation;
  }


  void setOrientation(Qt::Orientation orientation)
  {
    _layout_alg->setOrientation(orientation);
    handleConfigChange();
  }

  void setSpacing(qreal spacing)
  {
    _layout_alg->setSpacing(spacing);
    handleConfigChange();
  }


  void setFormat(QString format)
  {
    if (format.isEmpty() || format == _format)
      return;
    _format = std::move(format);
    handleConfigChange();
  }

  QString format() const noexcept { return _format; }


  void setTexturePerElement(bool enable)
  {
    _texture_per_element = enable;
    handleConfigChange();
  }
  bool texturePerElement() const noexcept { return _texture_per_element; }

  void setTextureStretch(bool enable)
  {
    _texture_stretch = enable;
    handleConfigChange();
  }
  bool textureStretch() const noexcept { return _texture_stretch; }

  void setTexture(QBrush b)
  {
    _texture = std::move(b);
    handleConfigChange();
  }
  const QBrush& texture() const noexcept { return _texture; }

  void setBackgroundPerElement(bool enable)
  {
    _background_per_element = enable;
    handleConfigChange();
  }
  bool backgroundPerElement() const noexcept { return _background_per_element; }

  void setBackgroundStretch(bool enable)
  {
    _background_stretch = enable;
    handleConfigChange();
  }
  bool backgroundStretch() const noexcept { return _background_stretch; }

  void setBackground(QBrush b)
  {
    _background = std::move(b);
    handleConfigChange();
  }
  const QBrush& background() const noexcept { return _background; }

  void setCachingEnabled(bool enable) noexcept { _caching_enabled = enable; }
  inline void enableCaching() noexcept { setCachingEnabled(true); }
  inline void disableCaching() noexcept { setCachingEnabled(false); }
  bool cachingEnabled() const noexcept { return _caching_enabled; }

private:
  void handleConfigChange();
  void updateConfigHash();

private:
  std::shared_ptr<ResourceFactory> _factory;
  std::shared_ptr<LinearLayout> _layout_alg;
  // public properties
  bool _supports_custom_separator = false;
  bool _supports_separator_animation = false;
  // effects configuration
  bool _texture_per_element = false;
  bool _background_per_element = false;
  bool _texture_stretch = false;
  bool _background_stretch = false;
  QBrush _texture = QColor(128, 64, 240);
  QBrush _background = QColor(240, 224, 64);
  // internal state
  bool _animate_separator = true;
  bool _separator_visible = true;
  bool _caching_enabled = true;
  QString _format;
  QString _separators;
  size_t _skin_cfg_hash = 0;
};
