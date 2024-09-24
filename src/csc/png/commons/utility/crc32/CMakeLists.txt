include(png_short_paths)

add_library(${CRC32} STATIC crc32.lib.cxx)
target_sources(${CRC32} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES crc32.lib.cxx)
target_link_libraries(${CRC32} PRIVATE ${BUFFER_VIEW} ZLIB::ZLIB stl_array)
