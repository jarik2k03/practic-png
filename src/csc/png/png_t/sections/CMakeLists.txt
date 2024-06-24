add_subdirectory(chunk)
add_subdirectory(attributes)

add_library(sections OBJECT sections.cxx)
target_link_libraries(sections PUBLIC chunk attributes)

target_compile_options(sections PRIVATE -fmodules-ts)
target_sources(sections PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES sections.cxx ) 

