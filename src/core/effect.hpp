#pragma once

#include <functional>

// TODO: forward declarations should be enough
#include <QPainter>
#include <QRectF>

class Effect {
public:
  virtual ~Effect() = default;

  using RenderItemFn = std::function<void(QPainter*)>;

  // p - output painter
  // r - item rect
  // render_item - function to render original item
  // TODO: declare own type for render_item
  void operator ()(QPainter* p, const QRectF& r,
                   RenderItemFn render_item) const
  {
    apply(p, r, std::move(render_item));
  }

private:
  virtual void apply(QPainter* p, const QRectF& r,
                     RenderItemFn render_item) const
  {
    Q_UNUSED(r);
    render_item(p);
  }
};


class SimpleEffect : public Effect {
private:
  void apply(QPainter* p, const QRectF& r,
             RenderItemFn render_item) const final
  {
    Q_UNUSED(render_item);
    simpleApply(p, r);
  }

  virtual void simpleApply(QPainter* p, const QRectF& r) const = 0;
};
