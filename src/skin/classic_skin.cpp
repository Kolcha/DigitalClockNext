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

#include "classic_skin.hpp"

#include "datetime_formatter.hpp"
#include "effects.hpp"
#include "hasher.hpp"
#include "layout.hpp"

namespace {

template<class Effect>
std::shared_ptr<Glyph> createEffect(std::shared_ptr<Glyph> inner, QBrush b, bool stretch)
{
  auto effect = std::make_shared<Effect>(std::move(inner));
  effect->setBrush(std::move(b));
  effect->setStretch(stretch);
  return effect;
}

std::shared_ptr<Glyph> buildEffectsStack(std::shared_ptr<Glyph> g, auto tx_cfg, auto bg_cfg)
{
  auto [bg, bg_stretch] = bg_cfg;
  auto [tx, tx_stretch] = tx_cfg;

  if (bg.style() == Qt::NoBrush && tx.style() == Qt::NoBrush) {
    // do nothing
  }
  if (bg.style() != Qt::NoBrush && tx.style() == Qt::NoBrush) {
    g = createEffect<BackgroundEffect>(std::move(g), std::move(bg), bg_stretch);
  }
  if (bg.style() == Qt::NoBrush && tx.style() != Qt::NoBrush) {
    g = createEffect<TexturingEffect>(std::move(g), std::move(tx), tx_stretch);
    g = std::make_shared<NewSurfaceEffect>(std::move(g));
  }
  if (bg.style() != Qt::NoBrush && tx.style() != Qt::NoBrush) {
    g = createEffect<TexturingEffect>(std::move(g), std::move(tx), tx_stretch);
    g = std::make_shared<NewSurfaceEffect>(std::move(g));
    g = createEffect<BackgroundEffect>(std::move(g), std::move(bg), bg_stretch);
  }

  return g;
}


class CacheKeyUpdater final : public GlyphDecorator {
public:
  CacheKeyUpdater(std::shared_ptr<Glyph> inner, size_t hash) noexcept
    : GlyphDecorator(std::move(inner))
    , _skin_cfg_hash(hash)
  {}

  size_t cacheKey() const override
  {
    return GlyphDecorator::cacheKey() ^ _skin_cfg_hash;
  }

  void setSkinConfigHash(size_t hash) noexcept { _skin_cfg_hash = hash; }

private:
  size_t _skin_cfg_hash = 0;
};


class ClassicLayoutBuilder final : public DateTimeStringBuilder {
public:
  ClassicLayoutBuilder(std::shared_ptr<ResourceFactory> factory,
                       std::shared_ptr<LayoutAlgorithm> layout_alg,
                       const ClassicSkin& skin)
    : _line(std::make_shared<CompositeGlyph>())
    , _factory(std::move(factory))
    , _skin(skin)
  {
    _line->setAlgorithm(std::move(layout_alg));
  }

  void addCharacter(QChar c) override
  {
    if (c == QLatin1Char('\n')) {
      if (!_layout) {
        _layout = std::make_shared<CompositeGlyph>();
        auto ca = std::dynamic_pointer_cast<LinearLayout>(_line->algorithm());
        Q_ASSERT(ca);   // classic layout can have only linear layout
        auto o = ca->orientation();
        o = o == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;
        auto alg = std::make_unique<LinearLayout>(o, ca->spacing());
        _layout->setAlgorithm(std::move(alg));
      }
      _line->updateGeometry();
      _layout->addGlyph(_line);
      _line = std::make_shared<CompositeGlyph>(_line->algorithm());
      return;
    }
    addItem(c);
  }

  void addSeparator(QChar c) override
  {
    if (_supports_custom_separator && _separator_idx < _separators.size())
      c = _separators[_separator_idx];

    ++_separator_idx;

    // separator animation
    bool separator_visible = true;
    if (_animate_separator) {
      if (!_separator_visible) {
        if (_supports_separator_animation)
          c = QLatin1Char(' ');
        else
          separator_visible = false;
      }
    }

    addItem(c)->setVisible(separator_visible);
  }

  void setSupportsCustomSeparator(bool supports) noexcept
  {
    _supports_custom_separator = supports;
  }

  void setSupportsSeparatorAnimation(bool supports) noexcept
  {
    _supports_separator_animation = supports;
  }

  void setCustomSeparators(QString separators) noexcept
  {
    _separators = std::move(separators);
  }

  void setSeparatorAnimationEnabled(bool enable) noexcept
  {
    _animate_separator = enable;
  }

  void setSeparatorVisible(bool visible) noexcept
  {
    _separator_visible = visible;
  }

  void setSkinConfigHash(size_t hash) noexcept { _skin_cfg_hash = hash; }

  std::shared_ptr<Glyph> getLayout()
  {
    if (_layout) {
      _line->updateGeometry();
      _layout->addGlyph(_line);
    } else {
      std::swap(_layout, _line);
    }
    Q_ASSERT(_layout->rect().isNull());
    _layout->updateGeometry();
    return buildLayoutStack(std::move(_layout));
  }

private:
  std::shared_ptr<Glyph> addItem(QChar c)
  {
    auto r = _factory->item(c);
    if (!r)
      return nullptr;
    auto item = buildItemStack(std::make_shared<SimpleGlyph>(std::move(r)));
    _line->addGlyph(item);
    return item;
  }

  std::shared_ptr<Glyph> buildItemStack(std::shared_ptr<Glyph> item) const
  {
    std::pair<QBrush, bool> tx;
    std::pair<QBrush, bool> bg;
    if (_skin.texturePerElement()) tx.first = _skin.texture();
    if (_skin.backgroundPerElement()) bg.first = _skin.background();
    tx.second = _skin.textureStretch();
    bg.second = _skin.backgroundStretch();
    item = buildEffectsStack(std::move(item), std::move(tx), std::move(bg));
    item = std::make_shared<CacheKeyUpdater>(std::move(item), _skin_cfg_hash);
    if (_skin.cachingEnabled()) item = std::make_shared<CachedGlyph>(item);
    return item;
  }

  std::shared_ptr<Glyph> buildLayoutStack(std::shared_ptr<Glyph> item) const
  {
    std::pair<QBrush, bool> tx;
    std::pair<QBrush, bool> bg;
    if (!_skin.texturePerElement()) tx.first = _skin.texture();
    if (!_skin.backgroundPerElement()) bg.first = _skin.background();
    tx.second = _skin.textureStretch();
    bg.second = _skin.backgroundStretch();
    return buildEffectsStack(std::move(item), std::move(tx), std::move(bg));
  }

private:
  std::shared_ptr<CompositeGlyph> _line;
  std::shared_ptr<CompositeGlyph> _layout;
  std::shared_ptr<ResourceFactory> _factory;
  const ClassicSkin& _skin;

  bool _supports_custom_separator = false;
  bool _supports_separator_animation = false;
  bool _animate_separator = true;
  bool _separator_visible = true;

  quint32 _separator_idx = 0;
  QString _separators;

  size_t _skin_cfg_hash = 0;
};

} // namespace

std::shared_ptr<Glyph> ClassicSkin::process(const QDateTime& dt)
{
  // TODO: consider to make it class member instead
  ClassicLayoutBuilder builder(_factory, _layout_alg, *this);
  builder.setSupportsCustomSeparator(supportsCustomSeparator());
  builder.setSupportsSeparatorAnimation(supportsSeparatorAnimation());
  builder.setCustomSeparators(_separators);
  builder.setSeparatorAnimationEnabled(_animate_separator);
  builder.setSeparatorVisible(_separator_visible);
  builder.setSkinConfigHash(_skin_cfg_hash);
  FormatDateTime(dt, _format, builder);
  return builder.getLayout();
}

void ClassicSkin::handleConfigChange()
{
  configurationChanged();
  updateConfigHash();
}

void ClassicSkin::updateConfigHash()
{
  _skin_cfg_hash = hasher(
      _texture, _texture_stretch, _texture_per_element,
      _background, _background_stretch, _background_per_element);
}
