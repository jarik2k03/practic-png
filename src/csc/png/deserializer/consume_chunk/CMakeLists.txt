add_subdirectory(inflater)
add_subdirectory(buf_reader)

add_library(consume_chunk_utility STATIC consume_chunk.utility.cxx)
target_link_libraries(consume_chunk_utility PRIVATE IHDR)
target_sources(consume_chunk_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.utility.cxx ) 

add_library(consume_chunk_overloads STATIC consume_chunk.overloads.cxx)
target_link_libraries(consume_chunk_overloads PRIVATE consume_chunk_utility inflater chunk buf_reader sections)
target_sources(consume_chunk_overloads PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.overloads.cxx ) 

add_library(consume_chunk STATIC consume_chunk.lib.cxx)
target_link_libraries(consume_chunk PRIVATE consume_chunk_overloads)
target_sources(consume_chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.lib.cxx ) 

