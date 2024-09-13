add_subdirectory(adler32)
add_subdirectory(memory_literals)
add_subdirectory(crc32)
add_subdirectory(endian)

add_library(utility STATIC utility.lib.cxx)
target_sources(utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES utility.lib.cxx ) 
target_link_libraries(utility PRIVATE adler32 crc32 endian memory_literals)
