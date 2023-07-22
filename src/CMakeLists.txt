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
        settings/fr_window_state.hpp
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
        settings/fr_app_global.hpp
        app/classic_skin_settings.cpp
        app/classic_skin_settings.hpp
        app/classic_skin_settings.ui
        app/settings_dialog.cpp
        app/settings_dialog.hpp
        app/settings_dialog.ui
        render/background_effect.hpp
        settings/sample_values.cpp
        settings/sample_values.hpp
        render/effects_hashing.cpp
        render/effects_hashing.hpp
        render/renderable_hashing.cpp
        render/renderable_hashing.hpp
        app/window_state.hpp
        settings/fr_general.hpp
        app/window_positioning.cpp
        app/window_positioning.hpp
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
