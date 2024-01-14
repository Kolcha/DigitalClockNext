qt_add_library(settings STATIC
    core/config_base.hpp
    core/settings.hpp
    core/state_base.hpp
    sections/app_global.hpp
    sections/appearance.hpp
    sections/classic_skin.cpp
    sections/classic_skin.hpp
    sections/debug.hpp
    sections/general.hpp
    sections/window_state.cpp
    sections/window_state.hpp
    app_config.hpp
    app_state.cpp
    app_state.hpp
    backend_qsettings.hpp
    config_base_qvariant.hpp
    conversion_qvariant.hpp
    state_base_qvariant.hpp
)
target_link_libraries(settings PUBLIC core)
target_link_libraries(settings PUBLIC Qt::Gui)
target_include_directories(settings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
