include(pngine_short_paths)

add_library(${SWAPCHAINKHR} STATIC swapchainKHR.lib.cxx)
target_sources(${SWAPCHAINKHR} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES swapchainKHR.lib.cxx )
target_link_libraries(${SWAPCHAINKHR} PRIVATE ${UTILITY}_swapchain_details ${UTILITY}_queue_family_indices vk_vulkan stl_stdexcept stl_vector)

