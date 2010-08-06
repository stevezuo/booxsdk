# Reset cxxflag as the onyx_cms library use third party libraries which have lot
# of compiling warnings.
SET(CMAKE_CXX_FLAGS_DEVEL
  "-Wall -Wno-sign-compare -Wctor-dtor-privacy  -Wold-style-cast -Woverloaded-virtual -g"
  CACHE STRING
  "Flags used by the C++ compiler during developer builds."
  FORCE)
SET(CMAKE_C_FLAGS_DEVEL "-Wall -pedantic -g" CACHE STRING
  "Flags used by the C compiler during developer builds."
  FORCE)


add_subdirectory(gtest)
add_subdirectory(gmock)
