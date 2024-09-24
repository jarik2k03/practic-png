include(png_short_paths)

add_subdirectory(picture)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_subdirectory(picture_debug)
endif() 
add_subdirectory(commons)
add_subdirectory(deserializer)
add_subdirectory(serializer)

add_library(png STATIC png.cxx)
target_link_libraries(png PUBLIC ${SERIALIZER} ${DESERIALIZER} ${PICTURE} )
target_sources(png PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png.cxx ) 
