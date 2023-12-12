qt_add_library(render STATIC
    effects.cpp
    effects.hpp
    font_resource.cpp
    font_resource.hpp
    image_resource.cpp
    image_resource.hpp
)
target_link_libraries(render PUBLIC core)
target_link_libraries(render PUBLIC Qt::Svg Qt::Gui)
target_include_directories(render INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
