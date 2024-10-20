include(png_short_paths)

add_subdirectory(IHDR)
add_subdirectory(PLTE)
add_subdirectory(IEND)
add_subdirectory(bKGD)
add_subdirectory(tIME)
add_subdirectory(tRNS)
add_subdirectory(cHRM)
add_subdirectory(gAMA)
add_subdirectory(hIST)
add_subdirectory(pHYs)

add_library(${SECTIONS} STATIC sections.cxx)
target_link_libraries(${SECTIONS} PRIVATE ${IHDR} ${PLTE} ${IEND} ${BKGD} ${TIME} ${CHRM} ${GAMA} ${HIST} ${PHYS} ${TRNS} stl_vector stl_variant)
target_sources(${SECTIONS} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES sections.cxx ) 

