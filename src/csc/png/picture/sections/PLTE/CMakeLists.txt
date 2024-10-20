include(png_short_paths)

add_library(${PLTE} STATIC PLTE.lib.cxx)
target_sources(${PLTE} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.lib.cxx ) 
target_link_libraries(${PLTE} PRIVATE stl_vector ${PIXEL_FORMATS})
