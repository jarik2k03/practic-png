add_library(gAMA STATIC gAMA.lib.cxx)
target_sources(gAMA PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES gAMA.lib.cxx ) 
# target_link_libraries(gAMA PRIVATE gAMA_attributes)

