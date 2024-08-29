add_subdirectory(write_section_to_ostream)

add_library(picture_debug STATIC picture_debug.lib.cxx)
target_sources(picture_debug PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES picture_debug.lib.cxx ) 
target_link_libraries(picture_debug PRIVATE write_section_to_ostream)
