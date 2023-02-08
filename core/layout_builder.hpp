#pragma once

#include <memory>
#include <utility>

#include "layout_item.hpp"

template<typename LayoutType>
class LayoutBuilder final {
public:
  template<typename... T>
  LayoutBuilder(T... args)
    : _layout(std::make_unique<LayoutTypeProxy>(std::forward<T>(args)...))
  {}

  LayoutBuilder& addItem(std::unique_ptr<LayoutItem> item)
  {
    _layout->addItem(std::move(item));
    return *this;
  }

  std::unique_ptr<LayoutType> build()
  {
    _layout->build();
    return std::move(_layout);
  }

private:
  class LayoutTypeProxy final : public LayoutType {
  public:
    using LayoutType::LayoutType;

    void addItem(std::unique_ptr<LayoutItem> item)
    {
      // Layout interface is assumed...
      // TODO: maybe also provide as specialization?
      LayoutType::addItem(std::move(item));
    }

    // no implementation, specialization must be provided
    void build();
  };
  std::unique_ptr<LayoutTypeProxy> _layout;
};
