add_library(stl_vector STATIC vector.cxx)
target_sources(stl_vector PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES vector.cxx) 

add_library(stl_array STATIC array.cxx)
target_sources(stl_array PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES array.cxx) 

add_library(stl_string_view STATIC string_view.cxx)
target_sources(stl_string_view PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES string_view.cxx) 

add_library(stl_ios STATIC ios.cxx)
target_sources(stl_ios PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES ios.cxx) 

add_library(stl_fstream STATIC fstream.cxx)
target_sources(stl_fstream PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES fstream.cxx) 

add_library(stl_istream STATIC istream.cxx)
target_sources(stl_istream PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES istream.cxx) 

add_library(stl_ostream STATIC ostream.cxx)
target_sources(stl_ostream PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES ostream.cxx) 

add_library(stl_iostream STATIC iostream.cxx)
target_sources(stl_iostream PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES iostream.cxx) 
target_link_libraries(stl_iostream PRIVATE stl_istream stl_ostream)

add_library(stl_stdexcept STATIC stdexcept.cxx)
target_sources(stl_stdexcept PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES stdexcept.cxx) 

add_library(stl_string STATIC string.cxx)
target_sources(stl_string PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES string.cxx) 

add_library(stl_variant STATIC variant.cxx)
target_sources(stl_variant PUBLIC FILE_SET csc_stl_wrap TYPE CXX_MODULES FILES variant.cxx) 