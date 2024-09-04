add_library(deflater_utility OBJECT deflater.utility.cxx)
target_sources(deflater_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.utility.cxx ) 
target_link_libraries(deflater_utility PRIVATE ZLIB::ZLIB buffer buffer_view stl_stdexcept)

add_library(deflater_attributes STATIC deflater.attributes.cxx)
target_link_libraries(deflater_attributes PRIVATE ZLIB::ZLIB)
target_sources(deflater_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.attributes.cxx) 

add_library(deflater_impl STATIC deflater.impl.cxx)
target_sources(deflater_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.impl.cxx ) 
target_link_libraries(deflater_impl PRIVATE deflater_utility deflater_attributes)

add_library(deflater STATIC deflater.lib.cxx)
target_sources(deflater PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.lib.cxx ) 
target_link_libraries(deflater PRIVATE deflater_impl)

