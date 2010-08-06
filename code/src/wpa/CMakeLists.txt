

IF (WIN32)
    SET(SRC wpa_ctrl.c os_win32.c)
ELSE (WIN32)
    SET(SRC wpa_ctrl.c os_unix.c)
ENDIF (WIN32)

ADD_LIBRARY(onyx_wpa ${SRC})
strict_warning(onyx_wpa)

