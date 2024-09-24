include(png_short_paths)

add_library(${BUFFER_VIEW}_range OBJECT buffer_view.range.cxx)
target_sources(${BUFFER_VIEW}_range PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer_view.range.cxx ) 

add_library(${BUFFER_VIEW} STATIC buffer_view.lib.cxx)
target_sources(${BUFFER_VIEW} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES buffer_view.lib.cxx ) 
target_link_libraries(${BUFFER_VIEW} PRIVATE ${BUFFER_VIEW}_range)


