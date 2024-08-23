add_subdirectory(sections)

add_library(picture_signature OBJECT picture.signature.cxx)
target_link_libraries(picture_signature PRIVATE stl_array)
target_sources(picture_signature PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES picture.signature.cxx ) 


add_library(picture STATIC picture.lib.cxx)
target_link_libraries(picture PRIVATE sections picture_signature)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
target_link_libraries(picture PRIVATE stl_iostream stl_ios)
endif()

target_sources(picture PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES picture.lib.cxx ) 

