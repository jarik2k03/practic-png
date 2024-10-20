include(pngine_short_paths)
add_library(${UTILITY}_swapchain_details STATIC swapchain_details.lib.cxx)
target_sources(${UTILITY}_swapchain_details PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES swapchain_details.lib.cxx )
target_link_libraries(${UTILITY}_swapchain_details PRIVATE vk_vulkan stl_vector)
