include(png_short_paths)

add_subdirectory(sections)

add_library(${PICTURE}_signature OBJECT picture.signature.cxx)
target_link_libraries(${PICTURE}_signature PRIVATE stl_array)
target_sources(${PICTURE}_signature PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES picture.signature.cxx )


add_library(${PICTURE} STATIC picture.lib.cxx)
target_link_libraries(${PICTURE} PRIVATE ${SECTIONS} ${PICTURE}_signature)

target_sources(${PICTURE} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES picture.lib.cxx )

