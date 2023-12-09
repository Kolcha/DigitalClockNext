qt_add_library(render STATIC
    effects/background.cpp
    effects/background.hpp
    effects/composite.cpp
    effects/composite.hpp
    effects/frame_border.cpp
    effects/frame_border.hpp
    effects/identity.hpp
    effects/texturing.cpp
    effects/texturing.hpp
    char_renderable.cpp
    char_renderable.hpp
    effect.hpp
    legacy_image_renderable.hpp
    rendering.cpp
    rendering.hpp
    skin_resource.hpp
    state_guard.hpp
    state_guard_qpainter.hpp
)
target_link_libraries(render PUBLIC core)
target_link_libraries(render PUBLIC Qt::Svg Qt::Gui)
target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
