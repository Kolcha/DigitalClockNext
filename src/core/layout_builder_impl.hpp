#pragma once

#include "layout_builder.hpp"

template<typename Layout>
template<typename Item>
void LayoutBuilder<Layout>::LayoutProxy::addItem(std::unique_ptr<Item> item)
{
  Layout::addItem(std::move(item));
}

template<typename Layout>
void LayoutBuilder<Layout>::LayoutProxy::build()
{
  this->updateGeometry();
}
