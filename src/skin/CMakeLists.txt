# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(skin STATIC
    classic_skin.cpp
    classic_skin.hpp
    classic_skin_loader.hpp
    datetime_formatter.cpp
    datetime_formatter.hpp
    error_skin.cpp
    error_skin.hpp
    legacy_skin_loader.cpp
    legacy_skin_loader.hpp
    observable.hpp
    skin.hpp
)
target_link_libraries(skin PUBLIC render core)
target_include_directories(skin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
