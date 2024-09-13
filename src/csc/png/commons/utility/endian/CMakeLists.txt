add_library(endian_attributes STATIC endian.attributes.cxx)
target_sources(endian_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES endian.attributes.cxx)

add_library(endian STATIC endian.lib.cxx)
target_sources(endian PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES endian.lib.cxx)
target_link_libraries(endian PRIVATE endian_attributes)

