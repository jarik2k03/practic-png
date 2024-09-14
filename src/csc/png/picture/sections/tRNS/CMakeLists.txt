add_library(tRNS_attributes STATIC tRNS.attributes.cxx)
target_link_libraries(tRNS_attributes PRIVATE stl_variant stl_vector pixel_formats)
target_sources(tRNS_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tRNS.attributes.cxx)

add_library(tRNS OBJECT tRNS.lib.cxx)
target_sources(tRNS PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES tRNS.lib.cxx )
target_link_libraries(tRNS PRIVATE tRNS_attributes)

