qt_add_library(skin STATIC
    char_renderable_factory.hpp
    classic_skin.cpp
    classic_skin.hpp
    classic_skin_loader.hpp
    clock_skin.hpp
    datetime_formatter.cpp
    datetime_formatter.hpp
    legacy_skin_loader.cpp
    legacy_skin_loader.hpp
    renderable_factory.hpp
)
target_link_libraries(skin PUBLIC render core)
target_include_directories(skin INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
