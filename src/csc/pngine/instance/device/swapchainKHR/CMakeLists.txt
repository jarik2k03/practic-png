include(pngine_short_paths)

add_library(${SWAPCHAINKHR}_utility OBJECT swapchainKHR.utility.cxx)
target_sources(${SWAPCHAINKHR}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES swapchainKHR.utility.cxx )
target_link_libraries(${SWAPCHAINKHR}_utility PRIVATE vk_vulkan stl_vector)

add_library(${SWAPCHAINKHR}_impl STATIC swapchainKHR.impl.cxx)
target_sources(${SWAPCHAINKHR}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES swapchainKHR.impl.cxx )
target_link_libraries(${SWAPCHAINKHR}_impl PRIVATE ${SWAPCHAINKHR}_utility ${UTILITY}_swapchain_details ${UTILITY}_queue_family_indices stl_stdexcept)

add_library(${SWAPCHAINKHR} STATIC swapchainKHR.lib.cxx)
target_sources(${SWAPCHAINKHR} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES swapchainKHR.lib.cxx )
target_link_libraries(${SWAPCHAINKHR} PRIVATE ${SWAPCHAINKHR}_impl)

