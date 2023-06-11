#pragma once

#include <memory>

#include "core/renderable_item.hpp"

size_t qHash(const Renderable& renderable, size_t seed = 0);
size_t qHash(const std::shared_ptr<Renderable>& r, size_t seed = 0);
size_t qHash(const RenderableItem& item, size_t seed = 0);
