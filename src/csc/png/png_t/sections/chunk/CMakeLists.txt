add_library(chunk OBJECT chunk.cxx)
target_sources(chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.cxx ) 
target_link_libraries(chunk PRIVATE stl_array stl_vector)
