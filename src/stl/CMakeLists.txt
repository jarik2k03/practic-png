add_library(stl_stringstream STATIC stringstream.cxx)
target_sources(stl_stringstream PUBLIC FILE_SET stl TYPE CXX_MODULES FILES stringstream.cxx)

add_library(stl_vector STATIC vector.cxx)
target_sources(stl_vector PUBLIC FILE_SET stl TYPE CXX_MODULES FILES vector.cxx)

add_library(stl_array STATIC array.cxx)
target_sources(stl_array PUBLIC FILE_SET stl TYPE CXX_MODULES FILES array.cxx)

add_library(stl_string_view STATIC string_view.cxx)
target_sources(stl_string_view PUBLIC FILE_SET stl TYPE CXX_MODULES FILES string_view.cxx)

add_library(stl_optional STATIC optional.cxx)
target_sources(stl_optional PUBLIC FILE_SET stl TYPE CXX_MODULES FILES optional.cxx)

add_library(stl_deque STATIC deque.cxx)
target_sources(stl_deque PUBLIC FILE_SET stl TYPE CXX_MODULES FILES deque.cxx)

add_library(stl_queue STATIC queue.cxx)
target_sources(stl_queue PUBLIC FILE_SET stl TYPE CXX_MODULES FILES queue.cxx)

add_library(stl_stack STATIC stack.cxx)
target_sources(stl_stack PUBLIC FILE_SET stl TYPE CXX_MODULES FILES stack.cxx)

add_library(stl_set STATIC set.cxx)
target_sources(stl_set PUBLIC FILE_SET stl TYPE CXX_MODULES FILES set.cxx)

add_library(stl_map STATIC map.cxx)
target_sources(stl_map PUBLIC FILE_SET stl TYPE CXX_MODULES FILES map.cxx)

add_library(stl_ios STATIC ios.cxx)
target_sources(stl_ios PUBLIC FILE_SET stl TYPE CXX_MODULES FILES ios.cxx)

add_library(stl_fstream STATIC fstream.cxx)
target_sources(stl_fstream PUBLIC FILE_SET stl TYPE CXX_MODULES FILES fstream.cxx)

add_library(stl_istream STATIC istream.cxx)
target_sources(stl_istream PUBLIC FILE_SET stl TYPE CXX_MODULES FILES istream.cxx)

add_library(stl_ostream STATIC ostream.cxx)
target_sources(stl_ostream PUBLIC FILE_SET stl TYPE CXX_MODULES FILES ostream.cxx)

add_library(stl_iostream STATIC iostream.cxx)
target_sources(stl_iostream PUBLIC FILE_SET stl TYPE CXX_MODULES FILES iostream.cxx)
target_link_libraries(stl_iostream PRIVATE stl_istream stl_ostream)

add_library(stl_stdexcept STATIC stdexcept.cxx)
target_sources(stl_stdexcept PUBLIC FILE_SET stl TYPE CXX_MODULES FILES stdexcept.cxx)

add_library(stl_string STATIC string.cxx)
target_sources(stl_string PUBLIC FILE_SET stl TYPE CXX_MODULES FILES string.cxx)

add_library(stl_variant STATIC variant.cxx)
target_sources(stl_variant PUBLIC FILE_SET stl TYPE CXX_MODULES FILES variant.cxx)
