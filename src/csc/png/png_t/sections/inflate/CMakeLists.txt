add_library(inflate OBJECT inflate.cxx)

target_compile_options(inflate PRIVATE -fmodules-ts)
target_sources(inflate PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES inflate.cxx ) 

