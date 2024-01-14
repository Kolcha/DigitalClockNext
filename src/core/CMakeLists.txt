qt_add_library(core STATIC
    geometry.hpp
    glyph.cpp
    glyph.hpp
    hasher.hpp
    layout.cpp
    layout.hpp
    layout_algorithm.hpp
    layout_debug.cpp
    layout_debug.hpp
    linear_layout.cpp
    linear_layout.hpp
    resource.hpp
)
target_link_libraries(core PUBLIC Qt::Gui)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
