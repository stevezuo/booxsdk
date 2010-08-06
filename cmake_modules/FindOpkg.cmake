# - Try to find the opkg library
# Once done this will define
#
#  OPKG_FOUND - system has opkg
#  OPKG_INCLUDE_DIR - the opkg include directory
#  OPKG_LIBRARIES - Link these to use opkg
#  OPKG_DEFINITIONS - Compiler switches required for using opkg
#

IF ( OPKG_INCLUDE_DIR AND OPKG_LIBRARIES )
    SET( OPKG_FIND_QUIETLY TRUE )
ENDIF ( OPKG_INCLUDE_DIR AND OPKG_LIBRARIES )

IF ( NOT WIN32 )
   INCLUDE( UsePkgConfig )
   PKGCONFIG( libopkg _LibOPKGIncDir _LibOPKGLinkDir _LibOPKGLinkFlags _LibOPKGCflags )

ENDIF ( NOT WIN32 )

# Look for opkg include dir and libraries
FIND_PATH( OPKG_INCLUDE_DIR libopkg/opkg.h PATHS ${_LibOPKGIncDir} )

FIND_LIBRARY( OPKG_LIBRARIES NAMES opkg PATHS ${_LibOPKGLinkDir} )

IF ( OPKG_INCLUDE_DIR AND OPKG_LIBRARIES )
        SET( OPKG_FOUND 1 )
        IF ( NOT OPKG_FIND_QUIETLY )
                MESSAGE ( STATUS "Found opkg: ${OPKG_LIBRARIES}" )
                MESSAGE ( STATUS "Include dir: ${OPKG_INCLUDE_DIR}")
        ENDIF ( NOT OPKG_FIND_QUIETLY )
ELSE ( OPKG_INCLUDE_DIR AND OPKG_LIBRARIES )
        IF ( NOT OPKG_FIND_QUIETLY )
                MESSAGE ( STATUS "Could NOT found opkg." )
        ENDIF ( NOT OPKG_FIND_QUIETLY )
ENDIF ( OPKG_INCLUDE_DIR AND OPKG_LIBRARIES )

# Hide advanced variables from CMake GUIs
MARK_AS_ADVANCED( OPKG_INCLUDE_DIR OPKG_LIBRARIES )
