# Define arm tool chain
MACRO (USE_ARM_TOOLCHAIN)
    MESSAGE("Use arm toolchain.")
    INCLUDE(CMakeForceCompiler)

    if ($ENV{ARM_CC})
         CMAKE_FORCE_C_COMPILER($ENV{ARM_CC} GNU)
         CMAKE_FORCE_CXX_COMPILER($ENV{ARM_CXX} GNU)
    else($ENV{ARM_CC})
         CMAKE_FORCE_C_COMPILER(arm-linux-gcc GNU)
         CMAKE_FORCE_CXX_COMPILER(arm-linux-g++ GNU)
    endif ($ENV{ARM_CC})

    # Define the dependency libraries root path.
    if ($ENV{ONYX_SDK_ROOT})
        message("Onyx SDK Root path $ENV{ONYX_SDK_ROOT}")
        SET(CMAKE_FIND_ROOT_PATH $ENV{ONYX_SDK_ROOT})
    else ($ENV{ONYX_SDK_ROOT})
        message("Use default path: /opt/onyx/arm/")
        SET(CMAKE_FIND_ROOT_PATH "/opt/onyx/arm/")
    endif ($ENV{ONYX_SDK_ROOT})
    SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    if (${CMAKE_FIND_ROOT_PATH})
        SET(CMAKE_INSTALL_PREFIX  ${CMAKE_FIND_ROOT_PATH})
    else (${CMAKE_FIND_ROOT_PATH})
        SET(CMAKE_INSTALL_PREFIX  "/opt/onyx/arm/")
    endif (${CMAKE_FIND_ROOT_PATH})

    # For libraries and headers in the target directories
    SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

    # Set up development build mode
    SET(CMAKE_CXX_FLAGS "-s ")
    SET(CMAKE_CXX_FLAGS_DEVEL "")
    SET(CMAKE_CXX_FLAGS_DEVEL
        "-Wall -Werror  -Wno-sign-compare  -Wno-ctor-dtor-privacy -Wno-long-long -Wno-non-virtual-dtor -s  "
        CACHE STRING
        "Flags used by the C++ compiler during developer builds."
        FORCE)
    SET(CMAKE_C_FLAGS_DEVEL "-Werror -Wall -pedantic " CACHE STRING
      "Flags used by the C compiler during developer builds."
      FORCE)
    #SET(CMAKE_EXE_LINKER_FLAGS_DEVEL
    #  "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING
    #  "Flags used for linking binaries during developer builds."
    #  FORCE)
    #SET(CMAKE_SHARED_LINKER_FLAGS_DEVEL
    #  "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING
    #  "Flags used by the shared libraries linker during developer builds."
    #  FORCE)
    #MARK_AS_ADVANCED(
    #  CMAKE_CXX_FLAGS_DEVEL
    #  CMAKE_C_FLAGS_DEVEL
    #  CMAKE_EXE_LINKER_FLAGS_DEVEL
    #  CMAKE_SHARED_LINKER_FLAGS_DEVEL)
    ## Update the documentation string of CMAKE_BUILD_TYPE for GUIs
    SET(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Devel."
        FORCE)

    SET(PDF_LIB libfpdfemb_arm.a)
    SET(TTS_LIB AiSound4)
    SET(ADD_LIB z m rt pthread dl)

    ADD_DEFINITIONS(-DBUILD_FOR_ARM)
    ADD_DEFINITIONS(-DENABLE_EINK_SCREEN)
    ADD_DEFINITIONS(-DBS60_INIT_MAIN)
    ADD_DEFINITIONS(-DBS80_INIT_MAIN)
    ADD_DEFINITIONS(-DBS97_INIT_MAIN)
    ADD_DEFINITIONS(-DSFM_M25P20)


    IF($ENV{HAS_TOUCH})
        ADD_DEFINITIONS(-DHAS_TOUCH)
    ELSE ($ENV{HAS_TOUCH})
        UNSET(HAS_TOUCH)
    ENDIF ($ENV{HAS_TOUCH})

ENDMACRO(USE_ARM_TOOLCHAIN)
