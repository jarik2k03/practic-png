add_library(utility STATIC utility.lib.cxx)
target_sources(utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES utility.lib.cxx ) 
# target_link_libraries(utility PRIVATE stl_vector)
