add_library(tIME_attributes OBJECT tIME.attributes.cxx)
target_sources(tIME_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tIME.attributes.cxx) 

add_library(tIME OBJECT tIME.lib.cxx)
target_sources(tIME PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tIME.lib.cxx ) 
target_link_libraries(tIME PRIVATE tIME_attributes)

