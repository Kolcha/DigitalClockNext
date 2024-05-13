# SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_sources(${PROJECT_NAME} PRIVATE
    about_dialog.cpp
    about_dialog.hpp
    about_dialog.ui
    application.cpp
    application.hpp
    application_config.cpp
    application_core.cpp
    application_private.hpp
    application_tray.cpp
    application_windows.cpp
    build_date.cpp
    build_date.hpp
    clock_icon_engine.cpp
    clock_icon_engine.hpp
    clock_tray_icon.cpp
    clock_tray_icon.hpp
    clock_widget.cpp
    clock_widget.hpp
    clock_window.cpp
    clock_window.hpp
    dialog_manager.hpp
    logo_label.cpp
    logo_label.hpp
    settings_manager.cpp
    settings_manager.hpp
    skin_manager.cpp
    skin_manager.hpp
    time_source.hpp
    update_checker.cpp
    update_checker.hpp
    window_positioning.cpp
    window_positioning.hpp
    window_state.hpp
)

add_subdirectory(platform)
add_subdirectory(settings)
