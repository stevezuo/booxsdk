enable_qt()

add_executable(battery_test
  battery_test.cpp
)
target_link_libraries(battery_test onyx_sys ${QT_LIBRARIES} m rt pthread dl)
set_target_properties(battery_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

add_executable(dump_wifi_profile
  dump_wifi_profile.cpp
)
target_link_libraries(dump_wifi_profile onyx_sys ${QT_LIBRARIES} m rt pthread dl)
set_target_properties(dump_wifi_profile PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

add_executable(wpa_proxy
  wpa_proxy.cpp
  ${UNITTEST_WPA_MOC_SRCS}
)
target_link_libraries(wpa_proxy onyx_sys ${QT_LIBRARIES} m rt pthread dl)
set_target_properties(wpa_proxy PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

qt4_wrap_cpp(MOC_SYS_STATUS_DUMP_SRCS sys_status_dump.h)
add_executable(sys_status_dump
  sys_status_dump.h
  sys_status_dump.cpp
  ${MOC_SYS_STATUS_DUMP_SRCS})
target_link_libraries(sys_status_dump onyx_sys ${QT_LIBRARIES} m rt pthread dl)
set_target_properties(sys_status_dump
  PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

#add_executable(scan_iwlist scan_iwlist.cpp)
#target_link_libraries(scan_iwlist onyx_sys ${QT_LIBRARIES})

add_executable(snapshot
  snapshot.cpp
)
target_link_libraries(snapshot onyx_sys ${QT_LIBRARIES} ${ADD_LIB})

add_executable(dump_device_info
  dump_device_info.cpp
)
target_link_libraries(dump_device_info onyx_sys ${QT_LIBRARIES} ${ADD_LIB})
set_target_properties(dump_device_info PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

add_executable(change_gray_scale
  change_gray_scale.cpp
)
target_link_libraries(change_gray_scale onyx_sys ${QT_LIBRARIES} ${ADD_LIB})
set_target_properties(change_gray_scale PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
