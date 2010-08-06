ENABLE_QT()

# Header files.
SET(HDRS
    ${ONYXSDK_DIR}/include/onyx/sys/sys_status.h
    ${ONYXSDK_DIR}/include/onyx/sys/wpa_connection.h
    ${ONYXSDK_DIR}/include/onyx/sys/messenger_proxy.h)
QT4_WRAP_CPP(MOC_SRCS ${HDRS})

# Source files.
FILE(GLOB SRCS *.cpp)
SET(SRCS ${HDRS} ${SRCS} ${MOC_SRCS})

ADD_LIBRARY(onyx_sys ${SRCS})
strict_warning(onyx_sys)
TARGET_LINK_LIBRARIES(onyx_sys onyx_wpa)


INSTALL(TARGETS onyx_sys DESTINATION lib)

