include(png_short_paths)

add_subdirectory(adler32)
add_subdirectory(memory_literals)
add_subdirectory(pixel_formats)
add_subdirectory(crc32)
add_subdirectory(endian)

add_library(${UTILITY} STATIC utility.lib.cxx)
target_sources(${UTILITY} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES utility.lib.cxx )
target_link_libraries(${UTILITY} PRIVATE ${ADLER32} ${CRC32} ${ENDIAN} ${MEMORY_LITERALS} ${PIXEL_FORMATS})
