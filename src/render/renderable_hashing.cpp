#include "renderable_hashing.hpp"

#include "render/char_renderable.hpp"
#include "render/layout_renderable.hpp"
#include "render/legacy_image_renderable.hpp"

#include "effects_hashing.hpp"

size_t qHashImpl(const QCharRenderable& r, size_t seed)
{
  return qHash(r._ch, seed) ^ qHash(r._font.toString(), seed);
}

size_t qHashImpl(const LegacyImageRenderable& r, size_t seed)
{
  return qHash(r.m_filename, seed);
}

namespace {

size_t qHashImpl(const LayoutRenderable& r, size_t seed)
{
  // LayoutRenderable may be dynamically allocated,
  // so pointer value may repeat after some time
  // use simple counter as 'unique identifier' for it
  // TODO: implement proper hashing
  static size_t counter = 0;
  return counter++ ^ seed;
}

} // namespace

size_t qHash(const Renderable& renderable, size_t seed)
{
  // TODO: consider public 'Hashable' interface
  if (auto r = dynamic_cast<const QCharRenderable*>(&renderable))
    return qHashImpl(*r, seed);
  if (auto r = dynamic_cast<const LegacyImageRenderable*>(&renderable))
    return qHashImpl(*r, seed);
  if (auto r = dynamic_cast<const LayoutRenderable*>(&renderable))
    return qHashImpl(*r, seed);
  Q_ASSERT_X(false, "qHash", "unknown renderable type");
  return 0;
}

size_t qHash(const std::shared_ptr<Renderable>& r, size_t seed)
{
  Q_ASSERT_X(r, "qHash", "renderable is null");
  return qHash(*r, seed);
}

size_t qHash(const RenderableItem& item, size_t seed)
{
  return qHash(item.renderable(), seed) ^ qHash(item.transform(), seed) ^ qHash(item.effects(), seed);
}
