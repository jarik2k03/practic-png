add_library(stl_vector STATIC vector.cxx)
target_sources(stl_vector PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES vector.cxx) 

add_library(stl_array STATIC array.cxx)
target_sources(stl_array PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES array.cxx) 

add_library(stl_string_view STATIC string_view.cxx)
target_sources(stl_string_view PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES string_view.cxx) 

add_library(stl_fstream STATIC fstream.cxx)
target_sources(stl_fstream PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES fstream.cxx) 

add_library(stl_stdexcept STATIC stdexcept.cxx)
target_sources(stl_stdexcept PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES stdexcept.cxx) 
