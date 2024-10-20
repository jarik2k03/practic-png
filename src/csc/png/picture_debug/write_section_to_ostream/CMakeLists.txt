include(png_short_paths)

add_library(${WRITE_SECTION_TO_OSTREAM}_overloads STATIC write_section_to_ostream.overloads.cxx)
target_sources(${WRITE_SECTION_TO_OSTREAM}_overloads PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES write_section_to_ostream.overloads.cxx ) 
target_link_libraries(${WRITE_SECTION_TO_OSTREAM}_overloads PRIVATE ${PICTURE} stl_ostream stl_ios)

add_library(${WRITE_SECTION_TO_OSTREAM} STATIC write_section_to_ostream.lib.cxx)
target_sources(${WRITE_SECTION_TO_OSTREAM} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES write_section_to_ostream.lib.cxx ) 
target_link_libraries(${WRITE_SECTION_TO_OSTREAM} PRIVATE ${WRITE_SECTION_TO_OSTREAM}_overloads)

