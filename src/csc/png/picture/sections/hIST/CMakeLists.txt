add_library(hIST_attributes OBJECT hIST.attributes.cxx)
target_sources(hIST_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES hIST.attributes.cxx) 

add_library(hIST OBJECT hIST.lib.cxx)
target_sources(hIST PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES hIST.lib.cxx ) 
target_link_libraries(hIST PRIVATE hIST_attributes stl_vector)

