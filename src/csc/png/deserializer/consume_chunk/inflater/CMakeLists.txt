add_library(inflater_utility OBJECT inflater.utility.cxx)
target_sources(inflater_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.utility.cxx ) 
target_link_libraries(inflater_utility PRIVATE buffer buffer_view stl_stdexcept ZLIB::ZLIB)

add_library(inflater_impl STATIC inflater.impl.cxx)
target_sources(inflater_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.impl.cxx ) 
target_link_libraries(inflater_impl PRIVATE inflater_utility memory_literals)

add_library(inflater STATIC inflater.lib.cxx)
target_sources(inflater PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflater.lib.cxx ) 
target_link_libraries(inflater PRIVATE inflater_impl)
