add_library(memory_literals STATIC memory_literals.lib.cxx)
target_sources(memory_literals PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES memory_literals.lib.cxx)
# target_link_libraries(memory_literals PRIVATE )

