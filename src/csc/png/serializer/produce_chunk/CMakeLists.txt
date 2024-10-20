include(png_short_paths)

add_subdirectory(buf_writer)
add_subdirectory(deflater)

add_library(${PRODUCE_CHUNK}_utility STATIC produce_chunk.utility.cxx)
target_link_libraries(${PRODUCE_CHUNK}_utility PRIVATE ${SECTIONS} ${BUFFER_VIEW} ${BUFFER} stl_array stl_variant)
target_sources(${PRODUCE_CHUNK}_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES produce_chunk.utility.cxx ) 

add_library(${PRODUCE_CHUNK}_overloads STATIC produce_chunk.overloads.cxx)
target_link_libraries(${PRODUCE_CHUNK}_overloads PRIVATE ${PRODUCE_CHUNK}_utility ${CHUNK} ${BUF_WRITER} ${CRC32})
target_sources(${PRODUCE_CHUNK}_overloads PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES produce_chunk.overloads.cxx ) 

add_library(${PRODUCE_CHUNK} STATIC produce_chunk.lib.cxx)
target_link_libraries(${PRODUCE_CHUNK} PRIVATE ${PRODUCE_CHUNK}_overloads)
target_sources(${PRODUCE_CHUNK} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES produce_chunk.lib.cxx ) 

