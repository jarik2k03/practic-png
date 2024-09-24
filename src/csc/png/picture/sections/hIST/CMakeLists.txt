include(png_short_paths)

add_library(${HIST}_attributes OBJECT hIST.attributes.cxx)
target_sources(${HIST}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES hIST.attributes.cxx) 

add_library(${HIST} OBJECT hIST.lib.cxx)
target_sources(${HIST} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES hIST.lib.cxx ) 
target_link_libraries(${HIST} PRIVATE ${HIST}_attributes stl_vector)

