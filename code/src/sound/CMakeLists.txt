ENABLE_QT()

# Header files.
FILE(GLOB hrds ${ONYXSDK_DIR}/include/onyx/sound/*.h)

# Source files.
QT4_WRAP_CPP(MOC_SRCS ${hrds})

FILE(GLOB srcs *.cpp)
SET(srcs ${srcs} ${hrds} ${MOC_SRCS})

# Sound library.
ADD_LIBRARY(sound ${srcs})

# Unittest
add_definitions(-DSAMPLE_ROOT="${CMAKE_SOURCE_DIR}/unittest/data")
FILE(GLOB WAV_UNITTEST_SRCS unittest/wave_unittest.cpp)
ADD_EXECUTABLE(wav_unittest ${WAV_UNITTEST_SRCS})
TARGET_LINK_LIBRARIES(wav_unittest sound onyx_sys
   gtest_main
   ${QT_LIBRARIES}
   ${ADD_LIB}
)
SET_TARGET_PROPERTIES(wav_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(WavUnittest ${TEST_OUTPUT_PATH}/wav_unittest)


# Async player
FILE(GLOB ASYNC_PLAYER_UNITTEST_SRCS unittest/async_player_unittest.cpp)

ADD_EXECUTABLE(async_player_unittest ${ASYNC_PLAYER_UNITTEST_SRCS})
TARGET_LINK_LIBRARIES(async_player_unittest sound onyx_sys
   gtest_main
   ${QT_LIBRARIES}
   ${ADD_LIB}
)

SET_TARGET_PROPERTIES(async_player_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
ADD_TEST(AsyncPlayerUnittest ${TEST_OUTPUT_PATH}/async_player_unittest)


# Sound unittest
FILE(GLOB SOUND_UNITTEST_SRCS unittest/sound_unittest.cpp)

ADD_EXECUTABLE(sound_unittest ${SOUND_UNITTEST_SRCS})
TARGET_LINK_LIBRARIES(sound_unittest sound onyx_sys
   gtest_main
   ${QT_LIBRARIES}
   ${ADD_LIB}
)

SET_TARGET_PROPERTIES(sound_unittest PROPERTIES  RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_PATH})
 ADD_TEST(SoundUnittest ${TEST_OUTPUT_PATH}/sound_unittest)

# For make install
#INSTALL(FILES ${hrds} DESTINATION include/onyx/sound)
INSTALL(TARGETS sound DESTINATION lib)

