add_library(IDAT_impl OBJECT IDAT.impl.cxx)
target_sources(IDAT_impl PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IDAT.impl.cxx ) 
target_link_libraries(IDAT_impl PRIVATE stl_vector stl_iostream IHDR inflater chunk utils )
 
add_library(IDAT STATIC IDAT.cxx)
target_sources(IDAT PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IDAT.cxx ) 
target_link_libraries(IDAT PRIVATE IDAT_impl)
