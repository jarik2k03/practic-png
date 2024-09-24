include(pngine_short_paths)
add_library(${UTILITY}_version STATIC version.lib.cxx)
target_sources(${UTILITY}_version PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES version.lib.cxx )
#target_link_libraries(version PRIVATE version_impl)

