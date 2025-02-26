add_library(png_commons_filtering_attributes OBJECT filtering.attributes.cxx)
target_sources(png_commons_filtering_attributes PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES filtering.attributes.cxx )

add_library(png_commons_filtering STATIC filtering.lib.cxx)
target_sources(png_commons_filtering PUBLIC FILE_SET csc_png TYPE CXX_MODULES FILES filtering.lib.cxx )
target_link_libraries(png_commons_filtering PRIVATE png_commons_filtering_attributes)

