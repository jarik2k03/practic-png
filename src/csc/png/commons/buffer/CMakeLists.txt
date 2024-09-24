include(png_short_paths)

add_library(${BUFFER}_range OBJECT buffer.range.cxx)
target_sources(${BUFFER}_range PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer.range.cxx ) 

add_library(${BUFFER} STATIC buffer.lib.cxx)
target_sources(${BUFFER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer.lib.cxx ) 
target_link_libraries(${BUFFER} PRIVATE ${BUFFER}_range stl_stdexcept)

