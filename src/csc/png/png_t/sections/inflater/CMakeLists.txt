add_library(inflater_impl OBJECT inflater.impl.cxx)
target_sources(inflater_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.impl.cxx ) 
target_link_libraries(inflater_impl PRIVATE ZLIB::ZLIB stl_vector)

add_library(inflater STATIC inflater.cxx)
target_sources(inflater PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.cxx ) 
target_link_libraries(inflater PRIVATE inflater_impl)