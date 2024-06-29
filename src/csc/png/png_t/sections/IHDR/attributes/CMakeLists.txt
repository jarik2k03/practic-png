add_library(IHDR_attributes OBJECT attributes.cxx)

target_compile_options(IHDR_attributes PRIVATE -fmodules-ts)
target_sources(IHDR_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES attributes.cxx ) 
