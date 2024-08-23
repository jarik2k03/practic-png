add_subdirectory(picture)
add_subdirectory(commons)
add_subdirectory(deserializer)
#add_subdirectory(serializer)

add_library(png STATIC png.cxx)
target_link_libraries(png PUBLIC deserializer picture )
target_sources(png PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png.cxx ) 
