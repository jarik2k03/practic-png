add_library(gAMA_attributes OBJECT gAMA.attributes.cxx)
target_sources(gAMA_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES gAMA.attributes.cxx) 

add_library(gAMA OBJECT gAMA.lib.cxx)
target_sources(gAMA PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES gAMA.lib.cxx ) 
target_link_libraries(gAMA PRIVATE gAMA_attributes)

