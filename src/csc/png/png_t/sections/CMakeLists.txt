add_subdirectory(chunk)
add_subdirectory(utils)
add_subdirectory(inflater)
add_subdirectory(IHDR)
add_subdirectory(PLTE)
add_subdirectory(IDAT)
add_subdirectory(IEND)

add_library(sections STATIC sections.cxx)
target_link_libraries(sections PRIVATE IHDR PLTE IDAT IEND stl_vector stl_array stl_variant)
target_sources(sections PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES sections.cxx ) 

