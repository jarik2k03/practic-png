add_library(inflater_attributes OBJECT inflater.attributes.cxx)
target_sources(inflater_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.attributes.cxx ) 

add_library(inflater_impl STATIC inflater.impl.cxx)
target_sources(inflater_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.impl.cxx ) 
target_link_libraries(inflater_impl PRIVATE ZLIB::ZLIB unique_buffer stl_stdexcept inflater_attributes)

add_library(inflater STATIC inflater.lib.cxx)
target_sources(inflater PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.lib.cxx ) 
target_link_libraries(inflater PRIVATE inflater_impl)
