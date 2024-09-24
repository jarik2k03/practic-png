include(pngine_short_paths)

add_library(${QUEUES}_utility STATIC queues.utility.cxx)
target_sources(${QUEUES}_utility PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES queues.utility.cxx)
target_link_libraries(${QUEUES}_utility PRIVATE vk_vulkan stl_optional)

target_sources(${QUEUES}_impl PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES queues.impl.cxx )
target_link_libraries(${QUEUES}_impl PRIVATE ${QUEUES}_utility stl_stdexcept stl_vector)

add_library(${QUEUES} STATIC queues.lib.cxx)
target_sources(${QUEUES} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES queues.lib.cxx )
target_link_libraries(${QUEUES} PRIVATE ${QUEUES}_impl)



