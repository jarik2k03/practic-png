include(pngine_short_paths)

add_subdirectory(queues)
add_subdirectory(swapchainKHR)
add_subdirectory(shader_module)
add_subdirectory(graphics_pipeline)

add_library(${DEVICE} STATIC device.lib.cxx)
target_sources(${DEVICE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.lib.cxx )
target_link_libraries(${DEVICE} PRIVATE ${QUEUES} ${SWAPCHAINKHR} ${SHADER_MODULE} ${GRAPHICS_PIPELINE} ${UTILITY}_swapchain_details)


