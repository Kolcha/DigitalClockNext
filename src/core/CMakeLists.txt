qt_add_library(core STATIC
    geometry.hpp
    glyph.cpp
    glyph.hpp
    layout.cpp
    layout.hpp
    layout_algorithm.hpp
    linear_layout.hpp
    resource.hpp
)
target_link_libraries(core PUBLIC Qt::Gui)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
