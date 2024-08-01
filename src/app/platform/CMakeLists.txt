# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    mouse_tracker.hpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(${PROJECT_NAME} PRIVATE
        win/fullscreen_detect.cpp
        win/fullscreen_detect.hpp
        win/mouse_tracker.cpp
        win/stay_on_top_hacks.cpp
        win/stay_on_top_hacks.hpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(${PROJECT_NAME} PRIVATE
        mac/mouse_tracker.mm
    )
else()
    target_sources(${PROJECT_NAME} PRIVATE
        mouse_tracker_impl_stub.cpp
    )
endif()
