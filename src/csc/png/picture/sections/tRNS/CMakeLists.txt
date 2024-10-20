include(png_short_paths)

add_library(${TRNS}_attributes STATIC tRNS.attributes.cxx)
target_link_libraries(${TRNS}_attributes PRIVATE stl_variant stl_vector ${PIXEL_FORMATS})
target_sources(${TRNS}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tRNS.attributes.cxx)

add_library(${TRNS} OBJECT tRNS.lib.cxx)
target_sources(${TRNS} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tRNS.lib.cxx )
target_link_libraries(${TRNS} PRIVATE ${TRNS}_attributes)

