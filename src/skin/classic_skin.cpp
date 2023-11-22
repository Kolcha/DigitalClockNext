#include "classic_skin.hpp"

#include "render/rendering.hpp"

namespace {

class ClassicSkinRenderable : public ClockRenderable {
public:
  using ClockRenderable::ClockRenderable;
};

} // namespace

std::shared_ptr<ClockRenderable> ClassicSkin::process(const QDateTime& dt)
{
  auto layout = std::make_shared<ClassicSkinRenderable>();
  layout->setAlgorithm(_layout_alg);

  const auto str = _formatter->process(dt);
  for (auto c : str) {
    // separator animation
    bool separator_visible = true;
    if (isSeparator(c) && _animate_separator) {
      if (!_separator_visible) {
        if (supportsSeparatorAnimation())
          c = QLatin1Char(' ');
        else
          separator_visible = false;
      }
    }

    auto r = _factory->item(c);
    if (!r) {
      continue;
    }

    auto item = std::make_shared<SimpleSkinElement>(std::move(r));
    if (isSeparator(c))
      item->setVisible(separator_visible);
    item->addEffect(_item_effects);
    layout->addElement(std::move(item));
  }

  layout->addEffect(_layout_effects);
  return layout;
}
