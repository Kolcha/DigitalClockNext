# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_sources(${PROJECT_NAME} PRIVATE
    app_global_settings.cpp
    app_global_settings.hpp
    app_global_settings.ui
    classic_skin_settings.cpp
    classic_skin_settings.hpp
    classic_skin_settings.ui
    debug_settings.cpp
    debug_settings.hpp
    debug_settings.ui
    settings_dialog.cpp
    settings_dialog.hpp
    settings_dialog.ui
    time_format_settings.cpp
    time_format_settings.hpp
    time_format_settings.ui
)
