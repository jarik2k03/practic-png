include(png_short_paths)

add_subdirectory(produce_chunk)

add_library(${SERIALIZER}_utility STATIC serializer.utility.cxx)
target_link_libraries(${SERIALIZER}_utility PRIVATE stl_fstream stl_vector ${CHUNK} ${PICTURE} ${BUF_WRITER} ${MEMORY_LITERALS})
target_sources(${SERIALIZER}_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.utility.cxx)

add_library(${SERIALIZER}_impl OBJECT serializer.impl.cxx)
target_link_libraries(${SERIALIZER}_impl PRIVATE ${SERIALIZER}_utility ${BUFFER_VIEW} ${PRODUCE_CHUNK} ${DEFLATER} stl_string_view stl_stdexcept stl_ios stl_string)
target_sources(${SERIALIZER}_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.impl.cxx)

add_library(${SERIALIZER} STATIC serializer.lib.cxx)
target_link_libraries(${SERIALIZER} PRIVATE ${SERIALIZER}_impl)
target_sources(${SERIALIZER} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES serializer.lib.cxx )

