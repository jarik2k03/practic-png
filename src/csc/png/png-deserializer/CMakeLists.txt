add_library(png-deserializer OBJECT png-deserializer.cxx)

target_link_libraries(png-deserializer PUBLIC sections)
target_compile_options(png-deserializer PRIVATE -fmodules-ts)
target_sources(png-deserializer PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES png-deserializer.cxx ) 

