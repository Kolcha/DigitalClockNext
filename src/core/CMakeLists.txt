qt_add_library(core STATIC
    hashable.hpp
    hasher.hpp
    item_geometry.hpp
    layout.hpp
    layout_algorithm.hpp
    layout_item.hpp
    linear_layout.hpp
)
target_link_libraries(core PUBLIC Qt::Gui)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
