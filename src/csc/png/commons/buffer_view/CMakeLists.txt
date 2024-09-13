add_library(buffer_view_range OBJECT buffer_view.range.cxx)
target_sources(buffer_view_range PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer_view.range.cxx ) 

add_library(buffer_view STATIC buffer_view.lib.cxx)
target_sources(buffer_view PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer_view.lib.cxx ) 
target_link_libraries(buffer_view PRIVATE buffer_view_range)


