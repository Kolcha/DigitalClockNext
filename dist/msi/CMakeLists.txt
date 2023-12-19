set(MSI_FILE "digital_clock_next-x64.msi")
set(MSI_SRC_DIR "${CMAKE_BINARY_DIR}/installer_files")

set(WIX_SRC_FILES
    main.wxs
    app_files.wxs
)

set(WIX_CANDLE_FLAGS "-nologo -arch x64 -ext WixUtilExtension")
string(APPEND WIX_CANDLE_FLAGS " -dAppVersion=${PROJECT_VERSION}")
string(APPEND WIX_CANDLE_FLAGS " -dAppIcon=${CMAKE_SOURCE_DIR}/dist/digital_clock.ico")
string(APPEND WIX_CANDLE_FLAGS " -dMySource=${MSI_SRC_DIR}")
set(WIX_LIGHT_FLAGS "-nologo -ext WixUtilExtension -spdb -sw1076")

list(TRANSFORM WIX_SRC_FILES REPLACE "\.wxs$" "\.wixobj" OUTPUT_VARIABLE WIX_OBJ_FILES)
list(TRANSFORM WIX_SRC_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

separate_arguments(WIX_CANDLE_FLAGS)
separate_arguments(WIX_LIGHT_FLAGS)

add_custom_command(
    OUTPUT ${MSI_SRC_DIR}/DigitalClockNext.exe
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${MSI_SRC_DIR}
    DEPENDS ${PROJECT_NAME}
)

add_custom_target(deploy_qt
    COMMAND windeployqt --libdir . --plugindir "plugins" --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw --no-translations DigitalClockNext.exe
    WORKING_DIRECTORY ${MSI_SRC_DIR}
    DEPENDS ${MSI_SRC_DIR}/DigitalClockNext.exe
)

add_custom_command(
    OUTPUT ${WIX_OBJ_FILES}
    COMMAND candle ${WIX_CANDLE_FLAGS} ${WIX_SRC_FILES}
    DEPENDS ${WIX_SRC_FILES}
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_command(
    OUTPUT ${MSI_FILE}
    COMMAND light ${WIX_LIGHT_FLAGS} -o ${MSI_FILE} ${WIX_OBJ_FILES}
    DEPENDS ${WIX_OBJ_FILES} deploy_qt
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_target(win_installer DEPENDS ${MSI_FILE} SOURCES ${WIX_SRC_FILES})
