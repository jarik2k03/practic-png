include(pngine_short_paths)

add_library(${DEBUG_REPORTEXT}_utility STATIC debug_reportEXT.utility.cxx)
target_sources(${DEBUG_REPORTEXT}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES debug_reportEXT.utility.cxx)
target_link_libraries(${DEBUG_REPORTEXT}_utility PRIVATE vk_vulkan stl_iostream)

add_library(${DEBUG_REPORTEXT}_impl STATIC debug_reportEXT.impl.cxx)
target_sources(${DEBUG_REPORTEXT}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES debug_reportEXT.impl.cxx )
target_link_libraries(${DEBUG_REPORTEXT}_impl PRIVATE ${DEBUG_REPORTEXT}_utility)

add_library(${DEBUG_REPORTEXT} STATIC debug_reportEXT.lib.cxx)
target_sources(${DEBUG_REPORTEXT} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES debug_reportEXT.lib.cxx )
target_link_libraries(${DEBUG_REPORTEXT} PRIVATE ${DEBUG_REPORTEXT}_impl)


