# Set up development build mode for x86
MACRO(USE_X86_TOOLCHAIN)

    SET(CMAKE_CXX_FLAGS_DEVEL
        "-Werror -Wall   -Wno-sign-compare -Wctor-dtor-privacy  -Woverloaded-virtual -g"
        CACHE STRING
        "Flags used by the C++ compiler during developer builds."
        FORCE)

    SET(CMAKE_C_FLAGS_DEVEL "-Werror -Wall -pedantic -g" CACHE STRING
        "Flags used by the C compiler during developer builds."
        FORCE)

    SET(CMAKE_EXE_LINKER_FLAGS_DEVEL
        "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING
        "Flags used for linking binaries during developer builds."
        FORCE)

    SET(CMAKE_SHARED_LINKER_FLAGS_DEVEL
        "-Wl,--warn-unresolved-symbols,--warn-once" CACHE STRING
        "Flags used by the shared libraries linker during developer builds."
        FORCE)

    MARK_AS_ADVANCED(
        CMAKE_CXX_FLAGS_DEVEL
        CMAKE_C_FLAGS_DEVEL
        CMAKE_EXE_LINKER_FLAGS_DEVEL
        CMAKE_SHARED_LINKER_FLAGS_DEVEL)

    # Update the documentation string of CMAKE_BUILD_TYPE for GUIs
    SET(CMAKE_BUILD_TYPE Devel CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Devel."
        FORCE)

ENDMACRO(USE_X86_TOOLCHAIN)
