add_library(IDAT OBJECT IDAT.cxx)

target_compile_options(IDAT PRIVATE -fmodules-ts)
target_link_libraries(IDAT PRIVATE IHDR chunk utils inflate )
target_sources(IDAT PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IDAT.cxx ) 

