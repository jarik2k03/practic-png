include(pngine_short_paths)
add_library(${UTILITY}_queue_family_indices STATIC queue_family_indices.lib.cxx)
target_sources(${UTILITY}_queue_family_indices PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES queue_family_indices.lib.cxx )
target_link_libraries(${UTILITY}_queue_family_indices PRIVATE vk_vulkan stl_optional)

