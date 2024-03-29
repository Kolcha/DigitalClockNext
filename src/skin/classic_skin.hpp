/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "skin.hpp"

#include <memory>

#include <QBrush>
#include <QString>

#include "resource_factory.hpp"

class ClassicSkinBase {
protected:  // TODO: move
  explicit ClassicSkinBase(std::shared_ptr<ResourceFactory> factory)
    : _factory(std::move(factory))
  {
    updateConfigHash();   // to set default value
  }

public:
  void setSupportsGlyphBaseHeight(bool supports) noexcept
  {
    _supports_glyph_base_height = supports;
  }
  bool supportsGlyphBaseHeight() const noexcept
  {
    return _supports_glyph_base_height;
  }

  void setOrientation(Qt::Orientation orientation);
  Qt::Orientation orientation() const noexcept { return _orienatation; }

  void setSpacing(qreal spacing)
  {
    _spacing = spacing;
    handleConfigChange();
  }

  qreal spacing() const noexcept { return _spacing; }


  void setIgnoreAdvanceX(bool enable);
  void setIgnoreAdvanceY(bool enable);

  bool ignoreAdvanceX() const noexcept { return _ignore_h_advance; }
  bool ignoreAdvanceY() const noexcept { return _ignore_v_advance; }


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

  void setGlyphBaseHeight(qreal h);

  void setLayoutConfig(QString layout_config);
  QString layoutConfig() const noexcept { return _layout_config; }

  void setCachingEnabled(bool enable) noexcept { _caching_enabled = enable; }
  inline void enableCaching() noexcept { setCachingEnabled(true); }
  inline void disableCaching() noexcept { setCachingEnabled(false); }
  bool cachingEnabled() const noexcept { return _caching_enabled; }

protected:
  virtual void handleConfigChange();
  void updateConfigHash();

protected:  // TODO: make private
  std::shared_ptr<ResourceFactory> _factory;
  // public properties
  bool _supports_glyph_base_height = true;
  // effects configuration
  bool _texture_per_element = false;
  bool _background_per_element = false;
  bool _texture_stretch = false;
  bool _background_stretch = false;
  QBrush _texture = QColor(128, 64, 240);
  QBrush _background = QColor(240, 224, 64);
  // internal state
  bool _caching_enabled = true;
  bool _ignore_h_advance = false;
  bool _ignore_v_advance = false;
  size_t _skin_cfg_hash = 0;
  Qt::Orientation _orienatation = Qt::Horizontal;
  qreal _spacing = 0.0;
  qreal _k_base_size = 1.0;
  QString _layout_config;
};


class StaticText final : public ClassicSkinBase {
public:
  explicit StaticText(std::shared_ptr<ResourceFactory> factory)
    : ClassicSkinBase(std::move(factory))
  {}

  std::shared_ptr<Resource> process(QStringView str) const;
};


class ClassicSkin final : public ClassicSkinBase, public Skin {
public:
  explicit ClassicSkin(std::shared_ptr<ResourceFactory> factory)
    : ClassicSkinBase(std::move(factory))
    , _format(QLatin1String("hh:mm a"))
  {}

  std::shared_ptr<Resource> process(const QDateTime& dt) override;

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

  void visit(SkinVisitor& visitor) override { visitor.visit(this); }

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
    _separators = separators.toUcs4();
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

  void setFormat(QString format)
  {
    if (format.isEmpty() || format == _format)
      return;
    _format = std::move(format);
    handleConfigChange();
  }

  QString format() const noexcept { return _format; }

  void setTokenTransform(QString token, QTransform transform);
  QTransform tokenTransform(const QString& token) const noexcept;

protected:
  void handleConfigChange() override;

private:
  // public properties
  bool _supports_custom_separator = false;
  bool _supports_separator_animation = false;
  // internal state
  bool _animate_separator = true;
  bool _separator_visible = true;
  QString _format;
  QList<uint> _separators;
  QHash<QString, QTransform> _token_transform;
};
