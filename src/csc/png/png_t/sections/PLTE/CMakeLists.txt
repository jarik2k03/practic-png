add_library(PLTE OBJECT PLTE.cxx)

target_compile_options(PLTE PRIVATE -fmodules-ts)
target_link_libraries(PLTE PRIVATE IHDR chunk utils )
target_sources(PLTE PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES PLTE.cxx ) 

