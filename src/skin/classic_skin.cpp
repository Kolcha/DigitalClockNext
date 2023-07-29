#include "classic_skin.hpp"

#include "render/rendering.hpp"

namespace {

class ClassicSkinRenderable : public ClockRenderable {
  friend class ::ClassicSkin;
public:
  using ClockRenderable::ClockRenderable;
};

} // namespace

std::shared_ptr<ClockRenderable> ClassicSkin::process(const QDateTime& dt)
{
  auto layout = std::make_shared<ClassicSkinRenderable>();
  std::vector<std::shared_ptr<SkinElement>> seps;
  layout->setAlgorithm(_layout_alg);

  const auto str = _formatter->process(dt);
  for (const auto& c : str) {
    auto r = _factory->item(c);
    if (!r) {
      continue;
    }

    auto item = std::make_shared<SimpleSkinElement>(std::move(r));
    if (isSeparator(c))
      seps.push_back(item);
    item->addEffect(_item_effects);
    layout->addElement(std::move(item));
  }

  layout->setSeparators(std::move(seps));
  layout->addEffect(_layout_effects);
  return layout;
}
