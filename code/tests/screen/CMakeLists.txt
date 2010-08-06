enable_qt()

add_executable(onyx_screen_test screen_test.cpp)
target_link_libraries(onyx_screen_test  onyx_screen ${QT_LIBRARIES}  ${ADD_LIB})
set_target_properties(onyx_screen_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
