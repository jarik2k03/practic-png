include(pngine_short_paths)

add_subdirectory(queues)

add_library(${DEVICE}_utility STATIC device.utility.cxx)
target_sources(${DEVICE}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.utility.cxx)
target_link_libraries(${DEVICE}_utility PRIVATE vk_vulkan stl_optional)

add_library(${DEVICE}_impl STATIC device.impl.cxx)
target_sources(${DEVICE}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.impl.cxx )
target_link_libraries(${DEVICE}_impl PRIVATE ${DEVICE}_utility stl_stdexcept stl_vector stl_optional ${QUEUES})

add_library(${DEVICE} STATIC device.lib.cxx)
target_sources(${DEVICE} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES device.lib.cxx )
target_link_libraries(${DEVICE} PRIVATE ${DEVICE}_impl)


