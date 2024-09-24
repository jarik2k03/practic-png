add_subdirectory(version)

include(pngine_short_paths)
add_library(${UTILITY} STATIC utility.lib.cxx)
target_sources(${UTILITY} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES utility.lib.cxx )
target_link_libraries(${UTILITY} PRIVATE ${UTILITY}_version)

