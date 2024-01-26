# SPDX-FileCopyrightText: 2023 Nick Korotysh <nick.korotysh@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(test_datetime_formatter test_datetime_formatter.cpp)
target_link_libraries(test_datetime_formatter PRIVATE skin)
target_link_libraries(test_datetime_formatter PRIVATE Qt::Test)
add_test(NAME test_datetime_formatter COMMAND test_datetime_formatter)

qt_add_executable(test_layout_item test_layout_item.cpp)
target_link_libraries(test_layout_item PRIVATE core)
target_link_libraries(test_layout_item PRIVATE Qt::Test)
add_test(NAME test_layout_item COMMAND test_layout_item)

qt_add_executable(test_layout test_layout.cpp)
target_link_libraries(test_layout PRIVATE core)
target_link_libraries(test_layout PRIVATE Qt::Test)
add_test(NAME test_layout COMMAND test_layout)

qt_add_executable(test_linear_layout test_linear_layout.cpp)
target_link_libraries(test_linear_layout PRIVATE core)
target_link_libraries(test_linear_layout PRIVATE Qt::Test)
add_test(NAME test_linear_layout COMMAND test_linear_layout)

qt_add_executable(test_settings_core test_settings_core.cpp)
target_link_libraries(test_settings_core PRIVATE settings)
target_link_libraries(test_settings_core PRIVATE Qt::Test)
add_test(NAME test_settings_core COMMAND test_settings_core)
