macro(STRICT_WARNING TARGET)
  if(NOT WIN32)
    if(NOT BUILD_FOR_ARM)
      if(DEBUG)
        set_target_properties(${TARGET}
          PROPERTIES
          COMPILE_FLAGS "-Wall -Werror -g -ggdb"
          CMAKE_EXE_LINKER_FLAGS "--no-undefined --fatal-warnings")
      else(DEBUG)
        set_target_properties(${TARGET}
          PROPERTIES
          COMPILE_FLAGS "-Wall -Werror"
          CMAKE_EXE_LINKER_FLAGS "--no-undefined --fatal-warnings")
      endif(DEBUG)
    endif(NOT BUILD_FOR_ARM)
  endif(NOT WIN32)
endmacro(STRICT_WARNING)
