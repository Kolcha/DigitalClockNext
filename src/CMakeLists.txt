cmake_minimum_required(VERSION 3.16)

project(digital_clock_next VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Svg)

set(PROJECT_SOURCES
        main.cpp
        core/layout_item.hpp
        core/layout.hpp
        core/linear_layout.hpp
        render/state_guard.hpp
        clock/clock_widget.hpp
        render/char_renderable.hpp
        skin/renderable_factory.hpp
        skin/char_renderable_factory.hpp
        skin/clock_skin.hpp
        skin/classic_skin.hpp
        render/effects/texturing.hpp
        skin/classic_skin_loader.hpp
        skin/legacy_skin_loader.hpp
        render/legacy_image_renderable.hpp
        render/effect.hpp
        render/effects/composite.hpp
        render/effects/identity.hpp
        clock/datetime_formatter.hpp
        settings/core/settings.hpp
        settings/backend_qsettings.hpp
        app/application.hpp
        app/application.cpp
        settings/core/config_base.hpp
        settings/app_config.hpp
        settings/sections/appearance.hpp
        settings/sections/classic_skin.hpp
        settings/sections/window_state.hpp
        settings/config_base_qvariant.hpp
        app/clock_window.cpp
        app/clock_window.hpp
        app/clock_widget.cpp
        app/clock_widget.hpp
        render/state_guard_qpainter.hpp
        clock/time_source.hpp
        clock/legacy_skin_extension.hpp
        app/application_private.hpp
        app/application_tray.cpp
        app/application_config.cpp
        app/application_core.cpp
        app/application_windows.cpp
        app/skin_manager.cpp
        app/skin_manager.hpp
        settings/sections/app_global.hpp
        app/classic_skin_settings.cpp
        app/classic_skin_settings.hpp
        app/classic_skin_settings.ui
        app/settings_dialog.cpp
        app/settings_dialog.hpp
        app/settings_dialog.ui
        render/effects/background.hpp
        settings/sections/sample_values.cpp
        settings/sections/sample_values.hpp
        app/window_state.hpp
        settings/sections/general.hpp
        app/window_positioning.cpp
        app/window_positioning.hpp
        core/geometry.hpp
        render/rendering.hpp
        render/skin_resource.hpp
        core/layout_algorithm.hpp
        render/effects/frame_border.hpp
        render/rendering.cpp
        core/hashable.hpp
        core/hasher.hpp
        render/effects/background.cpp
        render/effects/composite.cpp
        render/effects/frame_border.cpp
        render/effects/texturing.cpp
        render/char_renderable.cpp
        skin/classic_skin.cpp
        skin/legacy_skin_loader.cpp
)

qt_add_executable(digital_clock_next
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(digital_clock_next PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(digital_clock_next PRIVATE gradient_dialog)
target_link_libraries(digital_clock_next PRIVATE Qt6::Widgets Qt6::Svg)

set_target_properties(digital_clock_next PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS digital_clock_next
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_finalize_executable(digital_clock_next)
