add_subdirectory(chunk)
add_subdirectory(utils)
add_subdirectory(IHDR)
add_subdirectory(PLTE)
add_subdirectory(IDAT)
add_subdirectory(IEND)

add_library(sections STATIC sections.cxx)
target_link_libraries(sections PRIVATE IHDR PLTE IDAT IEND chunk utils )

target_compile_options(sections PRIVATE -fmodules-ts)
target_sources(sections PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES sections.cxx ) 

