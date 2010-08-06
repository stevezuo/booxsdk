ENABLE_QT()

SET(HDRS
  ${ONYXSDK_DIR}/include/onyx/wireless/dialup_dialog.h
  ${ONYXSDK_DIR}/include/onyx/wireless/ap_conf_dialog.h
  ${ONYXSDK_DIR}/include/onyx/wireless/wifi_dialog.h
  ${ONYXSDK_DIR}/include/onyx/wireless/ap_item.h)

SET(SRCS dialup_dialog.cpp wifi_dialog.cpp  ap_conf_dialog.cpp  ap_item.cpp)
SET(MOC_HDRS ${HDRS})

# Resources.
SET(resource_srcs wifi_images.qrc)
QT4_ADD_RESOURCES(WIFI_RES ${resource_srcs})

QT4_WRAP_CPP(MOC_SRCS ${HDRS})
ADD_LIBRARY(onyx_wireless ${SRCS} ${MOC_SRCS} ${WIFI_RES})
strict_warning(onyx_wireless)
INSTALL(TARGETS onyx_wireless DESTINATION lib)

