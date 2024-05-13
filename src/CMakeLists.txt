# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

configure_file(version.hpp.in version.hpp @ONLY)

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
)

add_subdirectory(app)
add_subdirectory(core)
add_subdirectory(render)
add_subdirectory(settings)
add_subdirectory(skin)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE skin settings)
