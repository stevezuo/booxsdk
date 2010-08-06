ENABLE_QT()

ADD_EXECUTABLE(hello_world hello_world.cpp)
TARGET_LINK_LIBRARIES(hello_world onyx_screen onyxapp onyx_sys ${QT_LIBRARIES})
set_target_properties(hello_world PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})

