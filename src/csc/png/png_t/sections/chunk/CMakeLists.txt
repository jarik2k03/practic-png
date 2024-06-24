add_library(chunk OBJECT chunk.cxx)

target_compile_options(chunk PRIVATE -fmodules-ts)
target_sources(chunk PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES chunk.cxx ) 

