cmake_minimum_required(VERSION 3.12)

project(font_rendering_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.hpp
        core/layout_item.hpp
        core/layout.hpp
        core/linear_layout.hpp
        core/renderable.hpp
        core/renderable_item.hpp
        core/layout_builder.hpp
        render/state_guard.hpp
        clock/clock_widget.hpp
        render/char_renderable.hpp
        skin/renderable_factory.hpp
        skin/char_renderable_factory.hpp
        skin/clock_skin.hpp
        skin/classic_skin.hpp
        render/layout_renderer.hpp
        render/texturing_effect.hpp
        skin/classic_skin_loader.hpp
        skin/legacy_skin_loader.hpp
        render/legacy_image_renderable.hpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(font_rendering_test
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET font_rendering_test APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(font_rendering_test SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(font_rendering_test
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(font_rendering_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(font_rendering_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg)

set_target_properties(font_rendering_test PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS font_rendering_test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(font_rendering_test)
endif()
