#include "classic_skin.hpp"

#include "clock/datetime_formatter.hpp"
#include "render/rendering.hpp"

namespace {

using ClassicSkinRenderable = ClockRenderable;

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
};

} // namespace

std::shared_ptr<ClockRenderable> ClassicSkin::process(const QDateTime& dt)
{
  // TODO: consider to make it class member instead
  ClassicLayoutBuilder builder(_factory, _layout_alg);
  builder.setItemEffects(_item_effects);
  builder.setLayoutEffects(_layout_effects);
  builder.setSupportsCustomSeparator(supportsCustomSeparator());
  builder.setSupportsSeparatorAnimation(supportsSeparatorAnimation());
  builder.setSeparatorAnimationEnabled(_animate_separator);
  builder.setSeparatorVisible(_separator_visible);
  FormatDateTime(dt, _format, builder);
  return builder.getLayout();
}
