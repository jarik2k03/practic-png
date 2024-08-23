add_library(IEND STATIC IEND.lib.cxx)
target_sources(IEND PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES IEND.lib.cxx ) 
