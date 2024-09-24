include(png_short_paths)

add_library(${CHRM} OBJECT cHRM.lib.cxx)
target_sources(${CHRM} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES cHRM.lib.cxx ) 

