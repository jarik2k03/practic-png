add_library(attributes STATIC attributes.lib.cxx)
target_link_libraries(attributes PRIVATE ZLIB::ZLIB)
target_sources(attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES attributes.lib.cxx) 

