# - Try to find the dbus library
# Once done this will define
#
#  DBUS_FOUND - system has sqlite
#  DBUS_INCLUDE_DIR - the sqlite include directory
#  DBUS_LIBRARIES - Link these to use sqlite
#  DBUS_DEFINITIONS - Compiler switches required for using sqlite
#

IF ( DBUS_INCLUDE_DIR AND DBUS_LIBRARIES )
    SET( DBUS_FIND_QUIETLY TRUE )
ENDIF ( DBUS_INCLUDE_DIR AND DBUS_LIBRARIES )

IF ( NOT WIN32 )
   INCLUDE( UsePkgConfig )
   PKGCONFIG( dbus-1 _LibDBusIncDir _LibDBUSLinkDir _LibDBUSLinkFlags _LibDBUSCflags )
ENDIF ( NOT WIN32 )

# Look for dbus-1 include dir and libraries
MESSAGE( ${_LibDBusIncDir})
FIND_PATH(DBUS_INCLUDE_DIR dbus/dbus.h ${_LibDBusIncDir} )
MESSAGE(${DBUS_INCLUDE_DIR})

FIND_LIBRARY( DBUS_LIBRARIES NAMES dbus-1 PATHS ${_LibDUBSLinkDir} )
MESSAGE(${DBUS_LIBRARIES})

IF ( DBUS_INCLUDE_DIR AND DBUS_LIBRARIES )
        SET( DBUS_FOUND 1 )
        IF ( NOT DBUS_FIND_QUIETLY )
                MESSAGE ( STATUS "Found DBus: ${DBUS_LIBRARIES}" )
        ENDIF ( NOT DBUS_FIND_QUIETLY )
ELSE ( DBUS_INCLUDE_DIR AND DBUS_LIBRARIES )
        IF ( NOT DBUS_FIND_QUIETLY )
                MESSAGE ( STATUS "Could NOT found dbus." )
        ENDIF ( NOT DBUS_FIND_QUIETLY )
ENDIF ( DBUS_INCLUDE_DIR AND DBUS_LIBRARIES )

# Hide advanced variables from CMake GUIs
MARK_AS_ADVANCED( DBUS_INCLUDE_DIR DBUS_LIBRARIES )
