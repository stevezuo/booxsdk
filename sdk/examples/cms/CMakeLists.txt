enable_qt()

add_executable(query_file query_file.cpp)
target_link_libraries(query_file onyx_cms ${QT_LIBRARIES})
set_target_properties(query_file PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
