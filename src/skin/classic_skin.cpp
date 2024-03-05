/*
 * SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "classic_skin.hpp"

#include "datetime_formatter.hpp"
#include "effects.hpp"
#include "hasher.hpp"
#include "linear_layout.hpp"

namespace {

template<class Effect>
std::shared_ptr<Resource> createEffect(std::shared_ptr<Resource> inner, QBrush b, bool stretch)
{
  auto effect = std::make_shared<Effect>(std::move(inner));
  effect->setBrush(std::move(b));
  effect->setStretch(stretch);
  return effect;
}

std::shared_ptr<Resource> buildEffectsStack(std::shared_ptr<Resource> g, auto tx_cfg, auto bg_cfg)
{
  auto [bg, bg_stretch] = bg_cfg;
  auto [tx, tx_stretch] = tx_cfg;

  if (bg.style() == Qt::NoBrush && tx.style() == Qt::NoBrush) {
    // do nothing
  }
  if (bg.style() != Qt::NoBrush && tx.style() == Qt::NoBrush) {
    g = createEffect<BackgroundDecorator>(std::move(g), std::move(bg), bg_stretch);
  }
  if (bg.style() == Qt::NoBrush && tx.style() != Qt::NoBrush) {
    g = createEffect<TexturingDecorator>(std::move(g), std::move(tx), tx_stretch);
    g = std::make_shared<NewSurfaceDecorator>(std::move(g));
  }
  if (bg.style() != Qt::NoBrush && tx.style() != Qt::NoBrush) {
    g = createEffect<TexturingDecorator>(std::move(g), std::move(tx), tx_stretch);
    g = std::make_shared<NewSurfaceDecorator>(std::move(g));
    g = createEffect<BackgroundDecorator>(std::move(g), std::move(bg), bg_stretch);
  }

  return g;
}

void applyLayoutConfig(LinearLayout& l, QStringView cfg) noexcept
{
  for (qsizetype i = 0; i < std::min<qsizetype>(l.items().size(), cfg.length()); i++) {
    switch (cfg[i].unicode()) {
      case '1':
        l.items()[i]->enableResize();
        break;
      case '<':
        l.setItemAlignment(i, Qt::AlignLeft | Qt::AlignTop);
        break;
      case '>':
        l.setItemAlignment(i, Qt::AlignRight | Qt::AlignBottom);
        break;
      case 'x':
      case 'X':
        l.setItemAlignment(i, Qt::AlignCenter);
        break;
      case '0':
      default:
        break;
    }
  }
}


class CacheKeyUpdater final : public ResourceDecorator {
public:
  CacheKeyUpdater(std::shared_ptr<Resource> inner, size_t hash) noexcept
    : ResourceDecorator(std::move(inner))
    , _skin_cfg_hash(hash)
  {}

  size_t cacheKey() const override
  {
    return ResourceDecorator::cacheKey() ^ _skin_cfg_hash;
  }

  void setSkinConfigHash(size_t hash) noexcept { _skin_cfg_hash = hash; }

private:
  size_t _skin_cfg_hash = 0;
};


class ResRectOverride final : public ResourceDecorator {
public:
  using ResourceDecorator::ResourceDecorator;

  QRectF rect() const override
  {
    return _rect.isValid() ? _rect : ResourceDecorator::rect();
  }

  void setRect(QRectF r) noexcept { _rect = std::move(r); }

private:
  QRectF _rect;
};


class ClassicLayoutBuilder : public DateTimeStringBuilder {
public:
  ClassicLayoutBuilder(std::shared_ptr<ResourceFactory> factory,
                       const ClassicSkinBase& skin)
    : _line(std::make_shared<LinearLayout>(skin.orientation(), skin.spacing()))
    , _factory(std::move(factory))
    , _skin(skin)
  {
    applyIgnoreAdvanceOptions(*_line);
  }

  void addCharacter(char32_t c) override
  {
    if (c == '\n') {
      if (!_layout) {
        auto o = _skin.orientation() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal;
        _layout = std::make_shared<LinearLayout>(o, _skin.spacing());
        applyIgnoreAdvanceOptions(*_layout);
      }
      addLine(std::move(_line));
      _line = std::make_shared<LinearLayout>(_skin.orientation(), _skin.spacing());
      applyIgnoreAdvanceOptions(*_line);
      return;
    }
    addItem(c);
  }

  void setSkinConfigHash(size_t hash) noexcept { _skin_cfg_hash = hash; }

  void setGlyphScaleFactor(qreal ks) noexcept { _ks = ks; }

  std::shared_ptr<Resource> getLayout()
  {
    std::shared_ptr<LayoutItem> layout;
    if (_layout) {
      addLine(std::move(_line));
      applyLayoutConfig(*_layout, _skin.layoutConfig());
      _layout->updateGeometry();
      layout = std::move(_layout);
    } else {
      std::swap(_layout, _line);
      layout = updateLineRect(std::move(_layout));
    }
    return buildLayoutStack(layout->resource());
  }

protected:
  void addItem(char32_t c, bool visible = true)
  {
    auto r = _factory->item(c);
    if (!r)
      return;
    if (visible)
      r = buildItemStack(std::move(r));
    else
      r = std::make_shared<InvisibleResource>(r->rect(), r->advanceX(), r->advanceY());
    auto item = std::make_shared<LayoutItem>(std::move(r));
    item->setTransform(itemTransform(c).scale(_ks, _ks));
    _line->addItem(std::move(item));
  }

  virtual QTransform itemTransform(char32_t c) const noexcept { return QTransform(); }

private:
  void addLine(std::shared_ptr<LinearLayout> line)
  {
    _layout->addItem(updateLineRect(std::move(line)));
    Q_ASSERT(_layout->rect().isNull());
  }

  // returns updated layout item that should be used instead
  // given layout item is not modified
  std::shared_ptr<LayoutItem> updateLineRect(std::shared_ptr<LayoutItem> line)
  {
    Q_ASSERT(line->rect().isNull());
    line->updateGeometry();
    if (_skin.ignoreAdvanceY()) return line;
    auto r = line->resource()->rect();
    // do not strictly rely on ascent/descent values
    // in case of Unicode characters not supported by selected font
    // some fallback font can be used, and it has different metrics
    r.setTop(std::min(r.top(), -_factory->ascent()));
    r.setBottom(std::max(r.bottom(), _factory->descent()));
    // why is it here? to preserve line height!
    auto res = std::make_shared<ResRectOverride>(line->resource());
    res->setRect(std::move(r));
    return std::make_shared<LayoutItem>(std::move(res));
  }

  void applyIgnoreAdvanceOptions(LinearLayout& l) const noexcept
  {
    if (l.orientation() == Qt::Horizontal) l.setIgnoreAdvance(_skin.ignoreAdvanceX());
    if (l.orientation() == Qt::Vertical) l.setIgnoreAdvance(_skin.ignoreAdvanceY());
  }

  std::shared_ptr<Resource> buildItemStack(std::shared_ptr<Resource> item) const
  {
    std::pair<QBrush, bool> tx;
    std::pair<QBrush, bool> bg;
    if (_skin.texturePerElement()) tx.first = _skin.texture();
    if (_skin.backgroundPerElement()) bg.first = _skin.background();
    tx.second = _skin.textureStretch();
    bg.second = _skin.backgroundStretch();
    item = buildEffectsStack(std::move(item), std::move(tx), std::move(bg));
    item = std::make_shared<CacheKeyUpdater>(std::move(item), _skin_cfg_hash);
    if (_skin.cachingEnabled()) item = std::make_shared<CachedResource>(item);
    return item;
  }

  std::shared_ptr<Resource> buildLayoutStack(std::shared_ptr<Resource> item) const
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
  std::shared_ptr<LinearLayout> _line;
  std::shared_ptr<LinearLayout> _layout;
  std::shared_ptr<ResourceFactory> _factory;
  const ClassicSkinBase& _skin;

  size_t _skin_cfg_hash = 0;

  qreal _ks = 1.0;
};


class DateTimeLayoutBuilder : public ClassicLayoutBuilder {
public:
  DateTimeLayoutBuilder(std::shared_ptr<ResourceFactory> factory,
                        const ClassicSkin& skin)
    : ClassicLayoutBuilder(std::move(factory), skin)
    , _skin(skin)
  {}

  void addSeparator(char32_t c) override
  {
    if (_supports_custom_separator && _separator_idx < _separators.size())
      c = _separators[_separator_idx];

    ++_separator_idx;

    // separator animation
    bool separator_visible = true;
    if (_animate_separator) {
      if (!_separator_visible) {
        if (_supports_separator_animation)
          c = ' ';
        else
          separator_visible = false;
      }
    }

    addItem(c, separator_visible);
  }

  void tokenStart(QStringView token) override
  {
    _current_token = token.toString();
  }

  void tokenEnd(QStringView token) override
  {
    Q_UNUSED(token)
    _current_token.clear();
  }

  void setSupportsCustomSeparator(bool supports) noexcept
  {
    _supports_custom_separator = supports;
  }

  void setSupportsSeparatorAnimation(bool supports) noexcept
  {
    _supports_separator_animation = supports;
  }

  void setCustomSeparators(QList<uint> separators) noexcept
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

protected:
  QTransform itemTransform(char32_t c) const noexcept override
  {
    Q_UNUSED(c);
    return _skin.tokenTransform(_current_token);
  }

private:
  const ClassicSkin& _skin;

  bool _supports_custom_separator = false;
  bool _supports_separator_animation = false;
  bool _animate_separator = true;
  bool _separator_visible = true;

  quint32 _separator_idx = 0;
  QList<uint> _separators;

  QString _current_token;
};

} // namespace

std::shared_ptr<Resource> ClassicSkin::process(const QDateTime& dt)
{
  DateTimeLayoutBuilder builder(_factory, *this);
  builder.setSupportsCustomSeparator(supportsCustomSeparator());
  builder.setSupportsSeparatorAnimation(supportsSeparatorAnimation());
  builder.setCustomSeparators(_separators);
  builder.setSeparatorAnimationEnabled(_animate_separator);
  builder.setSeparatorVisible(_separator_visible);
  builder.setSkinConfigHash(_skin_cfg_hash);
  builder.setGlyphScaleFactor(_k_base_size);
  FormatDateTime(dt, _format, builder);
  return builder.getLayout();
}

void ClassicSkin::setTokenTransform(QString token, QTransform transform)
{
  _token_transform[std::move(token)] = std::move(transform);
  handleConfigChange();
}

QTransform ClassicSkin::tokenTransform(const QString& token) const noexcept
{
  return _token_transform.value(token, QTransform());
}

void ClassicSkin::handleConfigChange()
{
  ClassicSkinBase::handleConfigChange();
  configurationChanged();
}

void ClassicSkinBase::setOrientation(Qt::Orientation orientation)
{
  _orienatation = orientation;
  handleConfigChange();
}

void ClassicSkinBase::setIgnoreAdvanceX(bool enable)
{
  _ignore_h_advance = enable;
  handleConfigChange();
}

void ClassicSkinBase::setIgnoreAdvanceY(bool enable)
{
  _ignore_v_advance = enable;
  handleConfigChange();
}

void ClassicSkinBase::setGlyphBaseHeight(qreal h)
{
  if (!supportsGlyphBaseHeight()) return;
  _k_base_size = h / _factory->height();
  handleConfigChange();
}

void ClassicSkinBase::setLayoutConfig(QString layout_config)
{
  if (layout_config == _layout_config)
    return;
  _layout_config = std::move(layout_config);
  handleConfigChange();
}

void ClassicSkinBase::handleConfigChange()
{
  updateConfigHash();
}

void ClassicSkinBase::updateConfigHash()
{
  _skin_cfg_hash = hasher(
      _texture, _texture_stretch, _texture_per_element,
      _background, _background_stretch, _background_per_element);
}

std::shared_ptr<Resource> StaticText::process(QStringView str) const
{
  ClassicLayoutBuilder builder(_factory, *this);
  builder.setSkinConfigHash(_skin_cfg_hash);
  builder.setGlyphScaleFactor(_k_base_size);
  const auto code_points = str.toUcs4();
  for (auto c : code_points) builder.addCharacter(c);
  return builder.getLayout();
}
