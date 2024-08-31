add_library(cHRM OBJECT cHRM.lib.cxx)
target_sources(cHRM PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES cHRM.lib.cxx ) 

