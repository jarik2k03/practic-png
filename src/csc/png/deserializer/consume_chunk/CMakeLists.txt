include(png_short_paths)

add_subdirectory(inflater)
add_subdirectory(buf_reader)

add_library(${CONSUME_CHUNK}_utility STATIC consume_chunk.utility.cxx)
target_link_libraries(${CONSUME_CHUNK}_utility PRIVATE ${IHDR})
target_sources(${CONSUME_CHUNK}_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.utility.cxx ) 

add_library(${CONSUME_CHUNK}_overloads STATIC consume_chunk.overloads.cxx)
target_link_libraries(${CONSUME_CHUNK}_overloads PRIVATE ${CONSUME_CHUNK}_utility ${CHUNK} ${BUF_READER} ${SECTIONS})
target_sources(${CONSUME_CHUNK}_overloads PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.overloads.cxx ) 

add_library(${CONSUME_CHUNK} STATIC consume_chunk.lib.cxx)
target_link_libraries(${CONSUME_CHUNK} PRIVATE ${CONSUME_CHUNK}_overloads stl_variant)
target_sources(${CONSUME_CHUNK} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.lib.cxx ) 

