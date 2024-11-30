include(pngine_short_paths)

add_subdirectory(queues)
add_subdirectory(swapchainKHR)
add_subdirectory(image_view)
add_subdirectory(shader_module)
add_subdirectory(graphics_pipeline)
add_subdirectory(render_pass)
add_subdirectory(framebuffer)
add_subdirectory(command_pool)
add_subdirectory(fence)
add_subdirectory(semaphore)

add_library(${DEVICE} STATIC device.lib.cxx)
target_sources(${DEVICE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.lib.cxx )
target_link_libraries(${DEVICE} PRIVATE stl_stdexcept stl_map ${QUEUES} ${IMAGE_VIEW} ${SWAPCHAINKHR} ${SHADER_MODULE} ${GRAPHICS_PIPELINE} ${RENDER_PASS} ${FRAMEBUFFER} ${COMMAND_POOL} ${FENCE} ${SEMAPHORE} ${UTILITY}_swapchain_details)


