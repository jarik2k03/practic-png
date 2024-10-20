include(png_short_paths)

add_library(${GAMA} STATIC gAMA.lib.cxx)
target_sources(${GAMA} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES gAMA.lib.cxx ) 
# target_link_libraries(${GAMA} PRIVATE ${GAMA}_attributes)

