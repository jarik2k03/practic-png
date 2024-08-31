add_subdirectory(consume_chunk)

add_library(deserializer_utility STATIC deserializer.utility.cxx)
target_link_libraries(deserializer_utility PRIVATE stl_string stl_fstream stl_stdexcept chunk picture buf_reader)
target_sources(deserializer_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.utility.cxx) 

add_library(deserializer_impl STATIC deserializer.impl.cxx)
target_link_libraries(deserializer_impl PRIVATE deserializer_utility consume_chunk inflater stl_string_view stl_ios)
target_sources(deserializer_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.impl.cxx) 

add_library(deserializer STATIC deserializer.lib.cxx)
target_link_libraries(deserializer PRIVATE deserializer_impl)
target_sources(deserializer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.lib.cxx ) 
