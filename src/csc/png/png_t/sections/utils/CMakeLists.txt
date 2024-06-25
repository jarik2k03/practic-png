add_library(utils OBJECT utils.cxx)

target_compile_options(utils PRIVATE -fmodules-ts)
target_sources(utils PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES utils.cxx ) 

