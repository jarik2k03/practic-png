include(png_short_paths)

add_library(${BKGD}_attributes STATIC bKGD.attributes.cxx)
target_link_libraries(${BKGD}_attributes PRIVATE stl_variant ${PIXEL_FORMATS})
target_sources(${BKGD}_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES bKGD.attributes.cxx)

add_library(${BKGD} OBJECT bKGD.lib.cxx)
target_sources(${BKGD} PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES bKGD.lib.cxx )
target_link_libraries(${BKGD} PRIVATE ${BKGD}_attributes)


