add_library(cHRM_attributes OBJECT cHRM.attributes.cxx)
target_sources(cHRM_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES cHRM.attributes.cxx) 

add_library(cHRM OBJECT cHRM.lib.cxx)
target_sources(cHRM PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES cHRM.lib.cxx ) 
target_link_libraries(cHRM PRIVATE cHRM_attributes)

