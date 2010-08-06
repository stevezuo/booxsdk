# For make install
FILE(GLOB_RECURSE hdr_files "*.h")
INSTALL(FILES ${hdr_files} DESTINATION include)
