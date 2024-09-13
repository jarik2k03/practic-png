add_library(crc32 STATIC crc32.lib.cxx)
target_sources(crc32 PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES crc32.lib.cxx)
target_link_libraries(crc32 PRIVATE buffer_view ZLIB::ZLIB stl_array)
