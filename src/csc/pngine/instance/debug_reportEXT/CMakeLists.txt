include(pngine_short_paths)

add_library(${DEBUG_REPORTEXT} STATIC debug_reportEXT.lib.cxx)
target_sources(${DEBUG_REPORTEXT} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES debug_reportEXT.lib.cxx )
target_link_libraries(${DEBUG_REPORTEXT} PRIVATE ${UTILITY}_debug_reportEXT)
