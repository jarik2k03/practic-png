add_subdirectory(IHDR)
add_subdirectory(PLTE)
add_subdirectory(IEND)
add_subdirectory(bKGD)
add_subdirectory(tIME)
add_subdirectory(cHRM)
add_subdirectory(gAMA)
add_subdirectory(hIST)
add_subdirectory(pHYs)

add_library(sections STATIC sections.cxx)
target_link_libraries(sections PRIVATE IHDR PLTE IEND bKGD tIME cHRM gAMA hIST pHYs stl_vector stl_variant)
target_sources(sections PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES sections.cxx ) 

