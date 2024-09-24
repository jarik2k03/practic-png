include(png_short_paths)

add_library(${IHDR}_attributes OBJECT IHDR.attributes.cxx)
target_sources(${IHDR}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IHDR.attributes.cxx) 

add_library(${IHDR} OBJECT IHDR.lib.cxx)
target_sources(${IHDR} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IHDR.lib.cxx ) 
target_link_libraries(${IHDR} PRIVATE ${IHDR}_attributes)

