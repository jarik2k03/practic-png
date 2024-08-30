add_library(unique_buffer OBJECT unique_buffer.lib.cxx)
target_sources(unique_buffer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES unique_buffer.lib.cxx ) 
# target_link_libraries(chunk PRIVATE stl_array)

