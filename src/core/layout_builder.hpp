#pragma once

#include <memory>
#include <utility>

template<typename LayoutType>
class LayoutBuilder final {
public:
  template<typename... T>
  LayoutBuilder& init(T... args)
  {
    _layout = std::make_unique<LayoutProxy>(std::forward<T>(args)...);
    return *this;
  }

  template<typename LayoutItem>
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
  class LayoutProxy final : public LayoutType {
  public:
    using LayoutType::LayoutType;

    // no implementation, specialization must be provided
    template<typename LayoutItem>
    void addItem(std::unique_ptr<LayoutItem> item);

    void build();
  };
  std::unique_ptr<LayoutProxy> _layout;
};
