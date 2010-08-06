
ENABLE_QT()

INCLUDE_DIRECTORIES(.)

SET(DICT_HDRS my_dict_plugin.h)
QT4_WRAP_CPP(MOC_SRCS ${DICT_HDRS})
SET(DICT_SRCS my_dict_plugin.cpp ${MOC_SRCS})

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)
add_definitions(-DQT_NO_DEBUG)

add_library(my_dict_plugin SHARED ${DICT_SRCS})
target_link_libraries(my_dict_plugin ${QT_LIBRARIES} ${ADD_LIB})


