add_subdirectory(produce_chunk)

add_library(serializer_utility STATIC serializer.utility.cxx)
target_link_libraries(serializer_utility PRIVATE stl_fstream stl_vector chunk picture buf_writer memory_literals)
target_sources(serializer_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.utility.cxx) 

add_library(serializer_impl OBJECT serializer.impl.cxx)
target_link_libraries(serializer_impl PRIVATE serializer_utility buffer_view produce_chunk deflater stl_string_view stl_stdexcept stl_ios stl_string)
target_sources(serializer_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.impl.cxx) 

add_library(serializer STATIC serializer.lib.cxx)
target_link_libraries(serializer PRIVATE serializer_impl)
target_sources(serializer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.lib.cxx ) 

