add_library(tIME STATIC tIME.lib.cxx)
target_sources(tIME PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tIME.lib.cxx ) 

