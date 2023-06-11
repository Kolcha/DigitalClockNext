#include "effects_hashing.hpp"

#include <algorithm>
#include <functional>

#include <QDataStream>
#include <QIODevice>

#include "render/background_effect.hpp"
#include "render/composite_effect.hpp"
#include "render/identity_effect.hpp"
#include "render/texturing_effect.hpp"

namespace {

using namespace Qt::Literals::StringLiterals;

size_t qHash(const QBrush& brush, size_t seed)
{
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  stream << brush;
  return qHash(buffer, seed);
}

size_t qHashImpl(const BackgroundEffect& effect, size_t seed)
{
  return qHash("BackgroundEffect"_L1, seed) ^ qHash(effect.brush(), seed);
}

size_t qHashImpl(const CompositeEffect& effect, size_t seed)
{
  return qHash("CompositeEffect"_L1, seed) ^ qHash(effect.effects(), seed);
}

size_t qHashImpl(const IdentityEffect& effect, size_t seed) noexcept
{
  Q_UNUSED(effect);
  return qHash("IdentityEffect"_L1, seed);
}

size_t qHashImpl(const TexturingEffect& effect, size_t seed)
{
  return qHash("TexturingEffect"_L1, seed) ^ qHash(effect.brush(), seed);
}

} // namespace

size_t qHash(const Effect& effect, size_t seed)
{
  if (auto e = dynamic_cast<const BackgroundEffect*>(&effect))
    return qHashImpl(*e, seed);
  if (auto e = dynamic_cast<const CompositeEffect*>(&effect))
    return qHashImpl(*e, seed);
  if (auto e = dynamic_cast<const IdentityEffect*>(&effect))
    return qHashImpl(*e, seed);
  if (auto e = dynamic_cast<const TexturingEffect*>(&effect))
    return qHashImpl(*e, seed);
  Q_ASSERT_X(false, "qHash", "unknown effect type");
  return 0;
}

size_t qHash(const std::shared_ptr<Effect>& effect, size_t seed)
{
  Q_ASSERT_X(effect, "qHash", "effect is null");
  return qHash(*effect, seed);
}

size_t qHash(const std::vector<std::shared_ptr<Effect>>& effects, size_t seed)
{
  auto hasher = [seed](const auto& e) { return qHash(e, seed); };
  return std::transform_reduce(effects.begin(), effects.end(),
                               static_cast<size_t>(0),
                               std::bit_xor<>(), hasher);
}
