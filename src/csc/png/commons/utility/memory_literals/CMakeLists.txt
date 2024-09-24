include(png_short_paths)

add_library(${MEMORY_LITERALS} STATIC memory_literals.lib.cxx)
target_sources(${MEMORY_LITERALS} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES memory_literals.lib.cxx)
# target_link_libraries(${MEMORY_LITERALS} PRIVATE )

