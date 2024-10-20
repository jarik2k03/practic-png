include(png_short_paths)

add_library(${ENDIAN}_attributes STATIC endian.attributes.cxx)
target_sources(${ENDIAN}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES endian.attributes.cxx)

add_library(${ENDIAN} STATIC endian.lib.cxx)
target_sources(${ENDIAN} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES endian.lib.cxx)
target_link_libraries(${ENDIAN} PRIVATE ${ENDIAN}_attributes)

