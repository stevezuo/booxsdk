enable_qt()

onyx_test(sys_conf_test sys_conf_test.cpp)
target_link_libraries(sys_conf_test  onyx_sys  gtest  ${QT_LIBRARIES}  ${ADD_LIB})



ADD_EXECUTABLE(wifi_manager_unittest wifi_manager_test.cpp)
TARGET_LINK_LIBRARIES(wifi_manager_unittest onyx_cms onyx_data onyx_sys ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(wifi_manager_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

ADD_EXECUTABLE(generate_default_peers generate_default_peers.cpp)
TARGET_LINK_LIBRARIES(generate_default_peers onyx_cms onyx_data onyx_sys ${QT_LIBRARIES})


