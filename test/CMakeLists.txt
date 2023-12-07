include_directories("../src")

qt_add_executable(test_item_geometry test_item_geometry.cpp)
target_link_libraries(test_item_geometry PRIVATE Qt::Test)
add_test(NAME test_item_geometry COMMAND test_item_geometry)

qt_add_executable(test_layout_item test_layout_item.cpp)
target_link_libraries(test_layout_item PRIVATE Qt::Test Qt::Gui)
add_test(NAME test_layout_item COMMAND test_layout_item)

qt_add_executable(test_layout test_layout.cpp)
target_link_libraries(test_layout PRIVATE Qt::Test Qt::Gui)
add_test(NAME test_layout COMMAND test_layout)

qt_add_executable(test_linear_layout test_linear_layout.cpp)
target_link_libraries(test_linear_layout PRIVATE Qt::Test Qt::Gui)
add_test(NAME test_linear_layout COMMAND test_linear_layout)
