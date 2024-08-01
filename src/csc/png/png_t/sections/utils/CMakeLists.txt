add_library(utils OBJECT utils.cxx)
target_sources(utils PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES utils.cxx ) 
target_link_libraries(utils PRIVATE stl_vector)
