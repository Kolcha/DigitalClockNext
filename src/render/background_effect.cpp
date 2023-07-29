#include "background_effect.hpp"

#include "core/hasher.hpp"

BackgroundEffect::BackgroundEffect(QBrush brush)
  : _brush(std::move(brush))
{
  updateHash(hasher(_brush));
}

void BackgroundEffect::setBrush(QBrush brush)
{
  _brush = std::move(brush);
  updateHash(hasher(_brush));
}
