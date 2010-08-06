enable_qt()

onyx_test(sys_conf_test sys_conf_test.cpp)
target_link_libraries(sys_conf_test  onyx_sys  gtest  ${QT_LIBRARIES}  ${ADD_LIB})
