include(png_short_paths)

add_library(${TIME} STATIC tIME.lib.cxx)
target_sources(${TIME} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tIME.lib.cxx ) 

