include(png_short_paths)

add_subdirectory(write_section_to_ostream)

add_library(${PICTURE_DEBUG} STATIC picture_debug.lib.cxx)
target_sources(${PICTURE_DEBUG} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES picture_debug.lib.cxx ) 
target_link_libraries(${PICTURE_DEBUG} PRIVATE ${WRITE_SECTION_TO_OSTREAM})
