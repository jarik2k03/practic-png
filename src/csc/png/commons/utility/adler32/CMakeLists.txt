include(png_short_paths)

add_library(${ADLER32} STATIC adler32.lib.cxx)
target_sources(${ADLER32} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES adler32.lib.cxx)
target_link_libraries(${ADLER32} PRIVATE ZLIB::ZLIB stl_array ${BUFFER_VIEW})
