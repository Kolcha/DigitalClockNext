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

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Network Svg)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
)

set(APP_IDENTIFIER "com.github.kolcha.DigitalClockNext")

add_subdirectory(deps)
add_subdirectory(dist)
add_subdirectory(res)
add_subdirectory(src)

find_package(Qt6 OPTIONAL_COMPONENTS Test)
if (${Qt6Test_FOUND})
    include(CTest)
    add_subdirectory(test)
endif()

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
