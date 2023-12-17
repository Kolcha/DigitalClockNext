set(MSI_FILE "digital_clock_next-x64.msi")
set(MSI_SRC_DIR "${CMAKE_BINARY_DIR}/installer_files")
set(WIX_OBJ_FILES main.wixobj all_files.wixobj)

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
    OUTPUT all_files.wxs
    COMMAND heat dir ${MSI_SRC_DIR} -gg -cg AllFiles -var var.MySource -srd -scom -sreg -svb6 -nologo -dr INSTALLDIR -out all_files.wxs
    DEPENDS deploy_qt
    VERBATIM
)

add_custom_command(
    OUTPUT main.wixobj
    COMMAND candle -nologo -arch x64 -dAppVersion=${PROJECT_VERSION} -ext WixUtilExtension -o main.wixobj ${CMAKE_CURRENT_SOURCE_DIR}/main.wxs
    DEPENDS main.wxs
    VERBATIM
)

add_custom_command(
    OUTPUT all_files.wixobj
    COMMAND candle -nologo -arch x64 -dMySource=${MSI_SRC_DIR} -o all_files.wixobj all_files.wxs
    DEPENDS all_files.wxs
    VERBATIM
)

add_custom_command(
    OUTPUT digital_clock.ico
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/dist/digital_clock.ico ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${PROJECT_NAME}
)

add_custom_command(
    OUTPUT ${MSI_FILE}
    COMMAND light -nologo -ext WixUtilExtension -spdb -sw1076 -o ${MSI_FILE} ${WIX_OBJ_FILES}
    DEPENDS ${WIX_OBJ_FILES} digital_clock.ico
    VERBATIM
    COMMAND_EXPAND_LISTS
)

add_custom_target(win_installer DEPENDS ${MSI_FILE} SOURCES main.wxs)
