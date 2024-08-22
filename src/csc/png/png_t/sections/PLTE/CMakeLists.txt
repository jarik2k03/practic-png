add_library(PLTE_attributes OBJECT PLTE.attributes.cxx)
target_sources(PLTE_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.attributes.cxx ) 

add_library(PLTE_impl OBJECT PLTE.impl.cxx)
target_sources(PLTE_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.impl.cxx ) 
target_link_libraries(PLTE_impl PRIVATE IHDR inflater chunk utils stl_vector PLTE_attributes)
 
add_library(PLTE STATIC PLTE.cxx)
target_sources(PLTE PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.cxx ) 
target_link_libraries(PLTE PRIVATE PLTE_impl)
