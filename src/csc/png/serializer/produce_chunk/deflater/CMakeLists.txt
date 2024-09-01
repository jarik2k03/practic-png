add_library(deflater_attributes OBJECT deflater.attributes.cxx)
target_sources(deflater_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.attributes.cxx ) 

add_library(deflater_impl STATIC deflater.impl.cxx)
target_sources(deflater_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.impl.cxx ) 
target_link_libraries(deflater_impl PRIVATE ZLIB::ZLIB unique_buffer stl_stdexcept stl_vector deflater_attributes)

add_library(deflater STATIC deflater.lib.cxx)
target_sources(deflater PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deflater.lib.cxx ) 
target_link_libraries(deflater PRIVATE deflater_impl)

