add_library(IEND_impl OBJECT IEND.impl.cxx)
target_sources(IEND_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IEND.impl.cxx) 
target_link_libraries(IEND_impl PRIVATE chunk utils stl_iostream)
 
add_library(IEND STATIC IEND.cxx)
target_sources(IEND PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IEND.cxx ) 
target_link_libraries(IEND PRIVATE IEND_impl)
