ENABLE_QT()

# Unittest
ADD_EXECUTABLE(wifi_gui_test wifi_gui_test.cpp ${WIFI_RES})
TARGET_LINK_LIBRARIES(wifi_gui_test
  onyx_wifi
  onyx_sys
  onyx_ui
  onyx_screen
  ${QT_LIBRARIES}
  ${ADD_LIB})
SET_TARGET_PROPERTIES(wifi_gui_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

# Unittest
ADD_EXECUTABLE(ap_conf_test ap_conf_main.cpp ${WIFI_RES})
TARGET_LINK_LIBRARIES(ap_conf_test
  onyx_wifi
  onyx_sys
  onyx_ui
  onyx_screen
  ${QT_LIBRARIES}
  ${ADD_LIB})
SET_TARGET_PROPERTIES(ap_conf_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

# Unittest
ADD_EXECUTABLE(dialup_test dialup_main.cpp ${WIFI_RES})
TARGET_LINK_LIBRARIES(dialup_test
  onyx_wifi
  onyx_sys
  onyx_ui
  onyx_screen
  ${QT_LIBRARIES}
  ${ADD_LIB})
SET_TARGET_PROPERTIES(dialup_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
