include(pngine_short_paths)

add_library(${SURFACEKHR} STATIC surfaceKHR.lib.cxx)
target_sources(${SURFACEKHR} PUBLIC FILE_SET csc_pngine TYPE CXX_MODULES FILES surfaceKHR.lib.cxx )
target_link_libraries(${SURFACEKHR} PRIVATE vk_vulkan)

