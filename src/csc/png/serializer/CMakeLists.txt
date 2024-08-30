add_subdirectory(produce_chunk)

add_library(serializer_impl OBJECT serializer.impl.cxx)
target_link_libraries(serializer_impl PRIVATE picture produce_chunk stl_string_view stl_fstream stl_stdexcept stl_string stl_ios stl_iostream stl_vector)
target_sources(serializer_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.impl.cxx) 

add_library(serializer STATIC serializer.lib.cxx)
target_link_libraries(serializer PRIVATE serializer_impl)
target_sources(serializer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.lib.cxx ) 

