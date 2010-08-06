enable_qt()

onyx_test(content_manager_unittest content_manager_unittest.cpp)
target_link_libraries(content_manager_unittest onyx_cms ${QT_LIBRARIES} gtest)

onyx_test(thumbnail_unittest thumbnail_unittest.cpp)
target_link_libraries(thumbnail_unittest onyx_cms ${QT_LIBRARIES} gtest)
