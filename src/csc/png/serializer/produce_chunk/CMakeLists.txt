add_subdirectory(buf_writer)
# add_subdirectory(deflater)

add_library(produce_chunk_utility STATIC produce_chunk.utility.cxx)
target_link_libraries(produce_chunk_utility PRIVATE sections)
target_sources(produce_chunk_utility PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES produce_chunk.utility.cxx ) 

add_library(produce_chunk_overloads STATIC produce_chunk.overloads.cxx)
target_link_libraries(produce_chunk_overloads PRIVATE produce_chunk_utility chunk buf_writer)
target_sources(produce_chunk_overloads PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES produce_chunk.overloads.cxx ) 

add_library(produce_chunk STATIC produce_chunk.lib.cxx)
target_link_libraries(produce_chunk PRIVATE produce_chunk_overloads)
target_sources(produce_chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES produce_chunk.lib.cxx ) 

