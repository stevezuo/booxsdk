# Make sure you already have cmake 2.6 installed. For cross compiling.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Additional cmake modules
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

# Naboo project.
Project(SDK CXX C)
ENABLE_TESTING()

# Global definitions
SET(ADD_LIB "")

# Check to use arm toolchain or not
IF(BUILD_FOR_ARM)
    INCLUDE(arm_toolchain)
    USE_ARM_TOOLCHAIN()
ELSE (BUILD_FOR_ARM)
    INCLUDE(x86_toolchain)
    USE_X86_TOOLCHAIN()
ENDIF(BUILD_FOR_ARM)

IF(UNIX OR BUILD_FOR_ARM)
    ADD_DEFINITIONS(-DSHARE_ROOT="/usr/share")
ELSE (UNIX OR BUILD_FOR_ARM)
    ADD_DEFINITIONS(-DSHARE_ROOT="")
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF (UNIX OR BUILD_FOR_ARM)

ADD_DEFINITIONS(-DCONFIG_CTRL_IFACE)
IF (WIN32)
ADD_DEFINITIONS(-DCONFIG_NATIVE_WINDOWS)
ADD_DEFINITIONS(-DCONFIG_CTRL_IFACE_NAMED_PIPE)
ELSE (WIN32)
ADD_DEFINITIONS(-DCONFIG_CTRL_IFACE_UNIX)
ENDIF (WIN32)

INCLUDE(strict_warning)

# Project include directories.
INCLUDE_DIRECTORIES(BEFORE
    ${CMAKE_SOURCE_DIR}/sdk/include
    ${CMAKE_SOURCE_DIR}/third_party/gtest/include
)

# Find thread library for the current platform
include(FindThreads)
include(enable_qt)
include(qt4_wrap_ts)
include(onyx_test)
include(misc)
include(tc)

LINK_DIRECTORIES(${QT_LIBRARY_DIR})

# Find ZLIB
INCLUDE(FindZLIB)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
LINK_DIRECTORIES(${ZLIB_LIBRARIES})

# Output directories.
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)
SET(TEST_OUTPUT_PATH ${PROJECT_BINARY_DIR}/unittests)
SET(QT_PLUGINS_OUTPUT_PATH ${PROJECT_BINARY_DIR}/plugins)

# All sub directories.
ADD_SUBDIRECTORY(third_party)
ADD_SUBDIRECTORY(sdk)

# Building qt plugins here.
# if(BUILD_FOR_ARM)
#     add_subdirectory(driver/screen)
#     add_subdirectory(driver/keyboard)
#     add_subdirectory(driver/kermit)
# endif(BUILD_FOR_ARM)

# Translation.
# QT4_WRAP_TS(lupdate_outputs lrelease_outputs
#   onyx
#   en_US fr_FR de_DE zh_CN zh_TW fi_FI es_ES pl_PL sv_SE nl_NL ru_RU gr_GR ja_JP
# )
# ADD_CUSTOM_TARGET(onyx_translations_target DEPENDS ${lupdate_outputs})


# Package the SDK
add_custom_target(sdk_dist
  ${CMAKE_SOURCE_DIR}/scripts/package_sdk
  DEPENDS onyxapp
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
