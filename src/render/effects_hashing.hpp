#pragma once

#include <memory>
#include <vector>

#include "core/effect.hpp"

size_t qHash(const Effect& effect, size_t seed = 0);
size_t qHash(const std::shared_ptr<Effect>& effect, size_t seed = 0);
size_t qHash(const std::vector<std::shared_ptr<Effect>>& effects, size_t seed = 0);
