add_library(chunk_attributes OBJECT chunk.attributes.cxx)
target_sources(chunk_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.attributes.cxx ) 

add_library(chunk STATIC chunk.lib.cxx)
target_sources(chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.lib.cxx ) 
target_link_libraries(chunk PRIVATE chunk_attributes stl_array buffer)
