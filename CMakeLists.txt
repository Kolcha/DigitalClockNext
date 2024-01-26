# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(DigitalClockNext
  VERSION 5.0.0
  DESCRIPTION "beautiful highly customizable clock"
  HOMEPAGE_URL "https://digitalclock4.sourceforge.io"
  LANGUAGES CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Network Svg)

set(PROJECT_SOURCES
        src/main.cpp
        src/app/application.hpp
        src/app/application.cpp
        src/app/clock_window.cpp
        src/app/clock_window.hpp
        src/app/clock_widget.cpp
        src/app/clock_widget.hpp
        src/app/time_source.hpp
        src/app/application_private.hpp
        src/app/application_tray.cpp
        src/app/application_config.cpp
        src/app/application_core.cpp
        src/app/application_windows.cpp
        src/app/skin_manager.cpp
        src/app/skin_manager.hpp
        src/app/settings/classic_skin_settings.cpp
        src/app/settings/classic_skin_settings.hpp
        src/app/settings/classic_skin_settings.ui
        src/app/settings/settings_dialog.cpp
        src/app/settings/settings_dialog.hpp
        src/app/settings/settings_dialog.ui
        src/app/window_state.hpp
        src/app/window_positioning.cpp
        src/app/window_positioning.hpp
        res/icons.qrc
        src/app/about_dialog.cpp
        src/app/about_dialog.hpp
        src/app/about_dialog.ui
        res/skins.qrc
        src/app/dialog_manager.hpp
        src/app/build_date.cpp
        src/app/build_date.hpp
        src/app/update_checker.cpp
        src/app/update_checker.hpp
        src/app/settings/app_global_settings.hpp
        src/app/settings/app_global_settings.cpp
        src/app/settings/app_global_settings.ui
        src/app/logo_label.cpp
        src/app/logo_label.hpp
        src/app/clock_icon_engine.cpp
        src/app/clock_icon_engine.hpp
        src/app/clock_tray_icon.cpp
        src/app/clock_tray_icon.hpp
        src/app/settings/debug_settings.cpp
        src/app/settings/debug_settings.hpp
        src/app/settings/debug_settings.ui
        src/app/settings_manager.cpp
        src/app/settings_manager.hpp
        src/app/settings/time_format_settings.cpp
        src/app/settings/time_format_settings.hpp
        src/app/settings/time_format_settings.ui
)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

set(APP_IDENTIFIER "com.github.kolcha.DigitalClockNext")
configure_file(src/version.hpp.in src/version.hpp @ONLY)

add_subdirectory(deps)
add_subdirectory(dist)
add_subdirectory(src/core)
add_subdirectory(src/render)
add_subdirectory(src/settings)
add_subdirectory(src/skin)

find_package(Qt6 OPTIONAL_COMPONENTS Test)
if (${Qt6Test_FOUND})
    include(CTest)
    add_subdirectory(test)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE skin settings)
target_link_libraries(${PROJECT_NAME} PRIVATE gradient_dialog)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Qt6::Svg)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "Digital Clock"
    MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_IDENTIFIER}
    MACOSX_BUNDLE_COPYRIGHT "(c) 2013-2024 Nick Korotysh"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_finalize_executable(${PROJECT_NAME})
