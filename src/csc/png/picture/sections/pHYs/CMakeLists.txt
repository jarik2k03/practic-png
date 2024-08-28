add_library(pHYs_attributes OBJECT pHYs.attributes.cxx)
target_sources(pHYs_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES pHYs.attributes.cxx) 

add_library(pHYs OBJECT pHYs.lib.cxx)
target_sources(pHYs PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES pHYs.lib.cxx ) 
target_link_libraries(pHYs PRIVATE pHYs_attributes)

