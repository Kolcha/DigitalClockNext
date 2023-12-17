if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WIN_ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/digital_clock.ico")
    configure_file(digital_clock.rc.in digital_clock.rc NEWLINE_STYLE CRLF)
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/digital_clock.rc
    )
    add_subdirectory(msi)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_ICON_FILE "digital_clock.icns"
    )

    target_sources(${PROJECT_NAME} PRIVATE "digital_clock.icns")
    set_source_files_properties(
        "digital_clock.icns"
            TARGET_DIRECTORY ${PROJECT_NAME}
            PROPERTIES
                MACOSX_PACKAGE_LOCATION Resources
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DESKTOP_FILE_NAME "${APP_IDENTIFIER}.desktop")
    configure_file(digital_clock.desktop.in ${DESKTOP_FILE_NAME})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications/
    )
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/digital_clock.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
    )
endif()
