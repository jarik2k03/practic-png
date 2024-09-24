include(png_short_paths)

add_library(${PHYS}_attributes OBJECT pHYs.attributes.cxx)
target_sources(${PHYS}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES pHYs.attributes.cxx) 

add_library(${PHYS} OBJECT pHYs.lib.cxx)
target_sources(${PHYS} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES pHYs.lib.cxx ) 
target_link_libraries(${PHYS} PRIVATE ${PHYS}_attributes)

