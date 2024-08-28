add_library(bKGD_attributes STATIC bKGD.attributes.cxx)
target_link_libraries(bKGD_attributes PRIVATE stl_variant) 
target_sources(bKGD_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES bKGD.attributes.cxx) 

add_library(bKGD OBJECT bKGD.lib.cxx)
target_sources(bKGD PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES bKGD.lib.cxx ) 
target_link_libraries(bKGD PRIVATE bKGD_attributes)


