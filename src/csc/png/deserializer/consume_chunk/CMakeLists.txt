add_subdirectory(inflater)

add_library(consume_chunk_overloads STATIC consume_chunk.overloads.cxx)
target_link_libraries(consume_chunk_overloads PRIVATE inflater sections utils chunk stl_variant)
target_sources(consume_chunk_overloads PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.overloads.cxx ) 

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  target_link_libraries(consume_chunk_overloads PRIVATE stl_iostream)
endif() 

add_library(consume_chunk STATIC consume_chunk.lib.cxx)
target_link_libraries(consume_chunk PRIVATE consume_chunk_overloads)
target_sources(consume_chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES consume_chunk.lib.cxx ) 

