include(pngine_short_paths)
add_library(${UTILITY}_debug_reportEXT STATIC debug_reportEXT.lib.cxx)
target_sources(${UTILITY}_debug_reportEXT PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES debug_reportEXT.lib.cxx )
target_link_libraries(${UTILITY}_debug_reportEXT PRIVATE vk_vulkan stl_iostream stl_optional)
