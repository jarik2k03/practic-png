include(png_short_paths)

add_library(${CHUNK}_attributes OBJECT chunk.attributes.cxx)
target_sources(${CHUNK}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.attributes.cxx ) 

add_library(${CHUNK} STATIC chunk.lib.cxx)
target_sources(${CHUNK} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.lib.cxx ) 
target_link_libraries(${CHUNK} PRIVATE ${CHUNK}_attributes stl_array ${BUFFER})
