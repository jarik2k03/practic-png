include(pngine_short_paths)

add_library(${QUEUES} STATIC queues.lib.cxx)
target_sources(${QUEUES} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES queues.lib.cxx )
target_link_libraries(${QUEUES} PRIVATE vk_vulkan stl_vector)



