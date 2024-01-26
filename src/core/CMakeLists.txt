# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(core STATIC
    hasher.hpp
    layout.cpp
    layout.hpp
    layout_debug.cpp
    layout_debug.hpp
    linear_layout.cpp
    linear_layout.hpp
    resource.cpp
    resource.hpp
)
target_link_libraries(core PUBLIC Qt::Gui)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
