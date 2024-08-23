add_library(PLTE_attributes OBJECT PLTE.attributes.cxx)
target_sources(PLTE_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.attributes.cxx ) 

add_library(PLTE STATIC PLTE.lib.cxx)
target_sources(PLTE PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.lib.cxx ) 
target_link_libraries(PLTE PRIVATE stl_vector PLTE_attributes)
