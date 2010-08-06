enable_qt()

ADD_EXECUTABLE(scoped_array_unittest scoped_array_unittest.cc)
TARGET_LINK_LIBRARIES(scoped_array_unittest gtest_main)
MAYBE_LINK_TCMALLOC(scoped_array_unittest)
SET_TARGET_PROPERTIES(scoped_array_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(ScopedArrayUnittest ${TEST_OUTPUT_PATH}/scoped_array_unittest)

ADD_EXECUTABLE(scoped_ptr_unittest scoped_ptr_unittest.cc)
TARGET_LINK_LIBRARIES(scoped_ptr_unittest gtest_main)
MAYBE_LINK_TCMALLOC(scoped_ptr_unittest)
SET_TARGET_PROPERTIES(scoped_ptr_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(ScopedPtrUnittest ${TEST_OUTPUT_PATH}/scoped_ptr_unittest)

ADD_EXECUTABLE(down_cast_unittest down_cast_unittest.cc)
TARGET_LINK_LIBRARIES(down_cast_unittest gtest_main)
MAYBE_LINK_TCMALLOC(down_cast_unittest)
set_target_properties(down_cast_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(DownCastUnittest ${TEST_OUTPUT_PATH}/down_cast_unittest)

ADD_EXECUTABLE(down_cast_negative_test down_cast_negative_test.cc)
TARGET_LINK_LIBRARIES(down_cast_negative_test gtest_main)
MAYBE_LINK_TCMALLOC(down_cast_negative_test)
SET_TARGET_PROPERTIES(down_cast_negative_test PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(DownCastNegativeTest ${TEST_OUTPUT_PATH}/down_cast_negative_test)

SET_TESTS_PROPERTIES(
  DownCastNegativeTest
  PROPERTIES
  WILL_FAIL TRUE)

onyx_test(conf_unittest conf_unittest.cpp configuration.cpp)
TARGET_LINK_LIBRARIES(conf_unittest onyx_cms onyx_data ${QT_LIBRARIES})
ADD_TEST(BookmarkUnittest ${TEST_OUTPUT_PATH}/conf_unittest)

ADD_EXECUTABLE(bookmark_unittest bookmark_unittest.cpp)
TARGET_LINK_LIBRARIES(bookmark_unittest onyx_cms gtest_main onyx_data ${QT_LIBRARIES})
MAYBE_LINK_TCMALLOC(bookmark_unittest)
SET_TARGET_PROPERTIES(bookmark_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(BookmarkUnittest ${TEST_OUTPUT_PATH}/bookmark_unittest)

add_subdirectory(sys)
add_subdirectory(cms)
