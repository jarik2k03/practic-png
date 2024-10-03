include(pngine_short_paths)

add_subdirectory(queues)
add_subdirectory(swapchainKHR)
add_subdirectory(image_view)
add_subdirectory(shader_module)

add_library(${DEVICE}_utility OBJECT device.utility.cxx)
target_sources(${DEVICE}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.utility.cxx)
target_link_libraries(${DEVICE}_utility PRIVATE vk_vulkan )

add_library(${DEVICE}_impl OBJECT device.impl.cxx)
target_sources(${DEVICE}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.impl.cxx )
target_link_libraries(${DEVICE}_impl PRIVATE ${DEVICE}_utility stl_stdexcept stl_map ${QUEUES} ${IMAGE_VIEW} ${SWAPCHAINKHR} ${SHADER_MODULE} ${UTILITY}_swapchain_details)

add_library(${DEVICE} STATIC device.lib.cxx)
target_sources(${DEVICE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.lib.cxx )
target_link_libraries(${DEVICE} PRIVATE ${DEVICE}_impl)


