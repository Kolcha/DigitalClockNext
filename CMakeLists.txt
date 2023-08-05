cmake_minimum_required(VERSION 3.16)

project(DigitalClockNext VERSION 5.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Svg)

set(PROJECT_SOURCES
        src/main.cpp
        src/core/layout_item.hpp
        src/core/layout.hpp
        src/core/linear_layout.hpp
        src/render/state_guard.hpp
        src/clock/clock_widget.hpp
        src/render/char_renderable.hpp
        src/skin/renderable_factory.hpp
        src/skin/char_renderable_factory.hpp
        src/skin/clock_skin.hpp
        src/skin/classic_skin.hpp
        src/render/effects/texturing.hpp
        src/skin/classic_skin_loader.hpp
        src/skin/legacy_skin_loader.hpp
        src/render/legacy_image_renderable.hpp
        src/render/effect.hpp
        src/render/effects/composite.hpp
        src/render/effects/identity.hpp
        src/clock/datetime_formatter.hpp
        src/settings/core/settings.hpp
        src/settings/backend_qsettings.hpp
        src/app/application.hpp
        src/app/application.cpp
        src/settings/core/config_base.hpp
        src/settings/app_config.hpp
        src/settings/sections/appearance.hpp
        src/settings/sections/classic_skin.hpp
        src/settings/sections/window_state.hpp
        src/settings/config_base_qvariant.hpp
        src/app/clock_window.cpp
        src/app/clock_window.hpp
        src/app/clock_widget.cpp
        src/app/clock_widget.hpp
        src/render/state_guard_qpainter.hpp
        src/clock/time_source.hpp
        src/clock/legacy_skin_extension.hpp
        src/app/application_private.hpp
        src/app/application_tray.cpp
        src/app/application_config.cpp
        src/app/application_core.cpp
        src/app/application_windows.cpp
        src/app/skin_manager.cpp
        src/app/skin_manager.hpp
        src/settings/sections/app_global.hpp
        src/app/classic_skin_settings.cpp
        src/app/classic_skin_settings.hpp
        src/app/classic_skin_settings.ui
        src/app/settings_dialog.cpp
        src/app/settings_dialog.hpp
        src/app/settings_dialog.ui
        src/render/effects/background.hpp
        src/settings/sections/sample_values.cpp
        src/settings/sections/sample_values.hpp
        src/app/window_state.hpp
        src/settings/sections/general.hpp
        src/app/window_positioning.cpp
        src/app/window_positioning.hpp
        src/core/geometry.hpp
        src/render/rendering.hpp
        src/render/skin_resource.hpp
        src/core/layout_algorithm.hpp
        src/render/effects/frame_border.hpp
        src/render/rendering.cpp
        src/core/hashable.hpp
        src/core/hasher.hpp
        src/render/effects/background.cpp
        src/render/effects/composite.cpp
        src/render/effects/frame_border.cpp
        src/render/effects/texturing.cpp
        src/render/char_renderable.cpp
        src/skin/classic_skin.cpp
        src/skin/legacy_skin_loader.cpp
)

qt_add_executable(digital_clock_next
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

add_subdirectory(dep)

target_include_directories(digital_clock_next PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
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
