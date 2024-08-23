add_library(IDAT STATIC IDAT.lib.cxx)
target_sources(IDAT PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IDAT.lib.cxx ) 
#target_link_libraries(IDAT PRIVATE IDAT_impl)
