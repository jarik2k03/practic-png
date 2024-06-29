add_library(IEND OBJECT IEND.cxx)

target_compile_options(IEND PRIVATE -fmodules-ts)
target_link_libraries(IEND PRIVATE chunk utils )
target_sources(IEND PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IEND.cxx ) 

