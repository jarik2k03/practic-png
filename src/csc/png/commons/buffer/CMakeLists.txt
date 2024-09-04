add_library(buffer_range OBJECT buffer.range.cxx)
target_sources(buffer_range PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer.range.cxx ) 

add_library(buffer STATIC buffer.lib.cxx)
target_sources(buffer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer.lib.cxx ) 
target_link_libraries(buffer PRIVATE buffer_range stl_stdexcept)

