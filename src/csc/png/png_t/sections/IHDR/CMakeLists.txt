add_subdirectory(attributes)
add_library(IHDR OBJECT IHDR.cxx)

target_compile_options(IHDR PRIVATE -fmodules-ts)
target_link_libraries(IHDR PRIVATE chunk utils IHDR_attributes )
target_sources(IHDR PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IHDR.cxx ) 

