macro(ONYX_TEST_WITHOUT_HEAPCHECK NAME SRC)
  add_executable(${NAME} ${SRC})
  target_link_libraries(${NAME} gtest_main)
  set_target_properties(${NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
  add_test(${NAME} ${TEST_OUTPUT_PATH}/${NAME})
endmacro(ONYX_TEST_WITHOUT_HEAPCHECK)

macro(ONYX_TEST NAME SRC)
  onyx_test_without_heapcheck(${NAME} ${SRC})
  maybe_link_tcmalloc(${NAME})
endmacro(ONYX_TEST)
