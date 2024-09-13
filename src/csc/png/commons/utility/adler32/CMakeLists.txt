add_library(adler32 STATIC adler32.lib.cxx)
target_sources(adler32 PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES adler32.lib.cxx)
target_link_libraries(adler32 PRIVATE ZLIB::ZLIB stl_array buffer_view)
