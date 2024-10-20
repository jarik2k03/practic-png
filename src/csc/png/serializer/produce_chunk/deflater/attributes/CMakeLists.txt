include(png_short_paths)

add_library(${DEFLATER_ATTRIBUTES} STATIC attributes.lib.cxx)
target_link_libraries(${DEFLATER_ATTRIBUTES} PRIVATE ZLIB::ZLIB)
target_sources(${DEFLATER_ATTRIBUTES} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES attributes.lib.cxx) 

