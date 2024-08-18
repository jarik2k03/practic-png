add_library(IHDR_attributes OBJECT IHDR.attributes.cxx)
target_sources(IHDR_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IHDR.attributes.cxx) 

add_library(IHDR_impl OBJECT IHDR.impl.cxx)
target_sources(IHDR_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IHDR.impl.cxx ) 
target_link_libraries(IHDR_impl PRIVATE chunk utils IHDR_attributes stl_iostream stl_vector)

add_library(IHDR STATIC IHDR.cxx)
target_sources(IHDR PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IHDR.cxx ) 
target_link_libraries(IHDR PRIVATE IHDR_attributes IHDR_impl)

