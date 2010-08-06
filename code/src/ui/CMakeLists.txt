ENABLE_QT()

# Header files.
FILE(GLOB HDRS ${ONYXSDK_DIR}/include/onyx/ui/*.h)
QT4_WRAP_CPP(MOC_SRCS ${HDRS})

# Source files.
FILE(GLOB SRCS *.cpp)
SET(SRCS ${HDRS} ${SRCS} ${MOC_SRCS})

# Resources.
SET(RESOURCE_SRCS onyx_ui_images.qrc)
QT4_ADD_RESOURCES(IMAGES_RES ${RESOURCE_SRCS})

SET(VBF_ICONS_RCCS
    vbf_icons.qrc)
QT4_ADD_RESOURCES(VBF_ICONS_RCCS_SRCS ${VBF_ICONS_RCCS})

IF (WIN32 OR ONYX_BUILD_STATIC_LIB)
    ADD_LIBRARY(onyx_ui ${SRCS} ${IMAGES_RES}  ${VBF_ICONS_RCCS_SRCS})
ELSE (WIN32 OR ONYX_BUILD_STATIC_LIB)
    ADD_LIBRARY(onyx_ui SHARED ${SRCS} ${IMAGES_RES}  ${VBF_ICONS_RCCS_SRCS})
ENDIF (WIN32 OR ONYX_BUILD_STATIC_LIB)

if (NOT WIN32)
strict_warning(onyx_ui)
endif (NOT WIN32)

TARGET_LINK_LIBRARIES(
  onyx_ui
  onyx_cms
  onyx_data
  onyx_screen
  onyx_sys
  ${QT_LIBRARIES}
  ${ADD_LIB})
