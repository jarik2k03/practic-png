add_library(chunk OBJECT chunk.lib.cxx)
target_sources(chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.lib.cxx ) 
target_link_libraries(chunk PRIVATE stl_array)
