add_library(deserializer_impl OBJECT deserializer.impl.cxx)
target_link_libraries(deserializer_impl PRIVATE png_t stl_string_view stl_fstream stl_stdexcept)
target_sources(deserializer_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.impl.cxx) 

add_library(deserializer STATIC deserializer.cxx)
target_link_libraries(deserializer PRIVATE deserializer_impl)
target_sources(deserializer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.cxx ) 
