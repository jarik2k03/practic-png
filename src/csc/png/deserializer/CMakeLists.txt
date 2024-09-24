include(png_short_paths)

add_subdirectory(consume_chunk)

add_library(${DESERIALIZER}_utility STATIC deserializer.utility.cxx)
target_link_libraries(${DESERIALIZER}_utility PRIVATE stl_string stl_fstream stl_stdexcept stl_optional ${INFLATER} ${CONSUME_CHUNK} ${CRC32} ${PICTURE} ${BUF_READER})
target_sources(${DESERIALIZER}_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.utility.cxx)

add_library(${DESERIALIZER}_impl STATIC deserializer.impl.cxx)
target_link_libraries(${DESERIALIZER}_impl PRIVATE ${DESERIALIZER}_utility stl_string_view stl_ios stl_iostream stl_vector)
target_sources(${DESERIALIZER}_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.impl.cxx)

add_library(${DESERIALIZER} STATIC deserializer.lib.cxx)
target_link_libraries(${DESERIALIZER} PRIVATE ${DESERIALIZER}_impl)
target_sources(${DESERIALIZER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES deserializer.lib.cxx )
