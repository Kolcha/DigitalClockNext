cmake_minimum_required(VERSION 3.16)

project(font_rendering_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Svg)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.hpp
        core/layout_item.hpp
        core/layout.hpp
        core/linear_layout.hpp
        core/renderable.hpp
        core/renderable_item.hpp
        core/layout_builder.hpp
        render/state_guard.hpp
        clock/clock_widget.hpp
        render/char_renderable.hpp
        skin/renderable_factory.hpp
        skin/char_renderable_factory.hpp
        skin/clock_skin.hpp
        skin/classic_skin.hpp
        render/layout_renderer.hpp
        render/texturing_effect.hpp
        skin/classic_skin_loader.hpp
        skin/legacy_skin_loader.hpp
        render/legacy_image_renderable.hpp
        core/effect.hpp
        render/renderable_base.hpp
        render/layout_renderable.hpp
        render/composite_effect.hpp
        render/identity_effect.hpp
        clock/datetime_formatter.hpp
        settings/settings.hpp
        settings/backend_qsettings.hpp
        app/application.hpp
        app/application.cpp
        settings/config_base.hpp
        settings/app_config.hpp
        settings/fr_appearance.hpp
        settings/fr_classic_skin.hpp
        settings/fr_state.hpp
        settings/config_base_qvariant.hpp
)

qt_add_executable(font_rendering_test
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(font_rendering_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(font_rendering_test PRIVATE Qt6::Widgets Qt6::Svg)

set_target_properties(font_rendering_test PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS font_rendering_test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_finalize_executable(font_rendering_test)
