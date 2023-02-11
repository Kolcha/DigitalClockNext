#pragma once

#include <functional>

class Renderable;
class QPainter;

// TODO: consider add public function to Renderable for that
using RenderItemFn = std::function<void(const Renderable*, QPainter*)>;

class Effect {
public:
  virtual ~Effect() = default;

  virtual void apply(const Renderable* item, QPainter* p, RenderItemFn r) const = 0;
};
