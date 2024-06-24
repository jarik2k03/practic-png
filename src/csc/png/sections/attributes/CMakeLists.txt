add_library(attributes OBJECT attributes.cxx)

target_compile_options(attributes PRIVATE -fmodules-ts)
target_sources(attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES attributes.cxx ) 
