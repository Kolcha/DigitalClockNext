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
#include "rendering.hpp"

namespace {

using ClassicSkinRenderable = LayoutSkinElement;

class ClassicLayoutBuilder final : public DateTimeStringBuilder {
public:
  ClassicLayoutBuilder(std::shared_ptr<RenderableFactory> provider,
                       std::shared_ptr<LayoutAlgorithm> layout_alg)
    : _layout(std::make_shared<ClassicSkinRenderable>())
    , _factory(std::move(provider))
  {
    _layout->setAlgorithm(std::move(layout_alg));
  }

  void addCharacter(QChar c) override
  {
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

  void setItemEffects(std::shared_ptr<CompositeEffect> effects) noexcept
  {
    _item_effects = std::move(effects);
  }

  void setLayoutEffects(std::shared_ptr<CompositeEffect> effects) noexcept
  {
    _layout_effects = std::move(effects);
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

  std::shared_ptr<ClockRenderable> getLayout()
  {
    Q_ASSERT(_layout->rect().isNull());
    _layout->updateGeometry();
    _layout->addEffect(_layout_effects);
    return std::move(_layout);
  }

private:
  std::shared_ptr<SkinElement> addItem(QChar c)
  {
    auto r = _factory->item(c);
    if (!r)
      return nullptr;
    auto item = std::make_shared<SimpleSkinElement>(std::move(r));
    item->addEffect(_item_effects);
    _layout->addElement(item);
    return item;
  }

private:
  std::shared_ptr<ClassicSkinRenderable> _layout;
  std::shared_ptr<RenderableFactory> _factory;
  std::shared_ptr<CompositeEffect> _item_effects;
  std::shared_ptr<CompositeEffect> _layout_effects;

  bool _supports_custom_separator = false;
  bool _supports_separator_animation = false;
  bool _animate_separator = true;
  bool _separator_visible = true;

  quint32 _separator_idx = 0;
  QString _separators;
};

} // namespace

std::shared_ptr<ClockRenderable> ClassicSkin::process(const QDateTime& dt)
{
  // TODO: consider to make it class member instead
  ClassicLayoutBuilder builder(_factory, _layout_alg);
  builder.setItemEffects(_item_effects);
  builder.setLayoutEffects(_layout_effects);
  builder.setSupportsCustomSeparator(supportsCustomSeparator());
  builder.setSupportsSeparatorAnimation(_supports_separator_animation);
  builder.setCustomSeparators(_separators);
  builder.setSeparatorAnimationEnabled(_animate_separator);
  builder.setSeparatorVisible(_separator_visible);
  FormatDateTime(dt, _format, builder);
  return builder.getLayout();
}
